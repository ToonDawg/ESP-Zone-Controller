#include "AzureIoTManager.h"
#include "iot_configs.h"
#include <azure_ca.h>
#include "ArduinoJson.h"

static esp_mqtt_client_handle_t mqtt_client;
static az_iot_hub_client client;
static char incoming_data[INCOMING_DATA_BUFFER_SIZE];
static const char *host = IOT_CONFIG_IOTHUB_FQDN;
static const char *device_id = IOT_CONFIG_DEVICE_ID;
static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[200];
static const char *mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
static uint8_t sas_signature_buffer[256];
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;
static uint32_t telemetry_send_count = 0;
static char telemetry_topic[128];
static unsigned long next_telemetry_send_time_ms = 0;
bool AzureIoTManager::sendTelemetryOnRequest = false;

static AzIoTSasToken sasToken(
    &client,
    AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_KEY),
    AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
    AZ_SPAN_FROM_BUFFER(mqtt_password));

static void connectToWiFi()
{
    Logger.Info("Connecting to WIFI SSID " + String(IOT_CONFIG_WIFI_SSID));

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(IOT_CONFIG_WIFI_SSID, IOT_CONFIG_WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");

    Logger.Info("WiFi connected, IP address: " + WiFi.localIP().toString());
}

static void initializeTime()
{
    Logger.Info("Setting time using SNTP");

    configTime(GMT_OFFSET_SECS, GMT_OFFSET_SECS_DST, NTP_SERVERS);
    time_t now = time(NULL);
    while (now < UNIX_TIME_NOV_13_2017)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    Logger.Info("Time initialized!");
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
    Logger.Info("Received [");
    Logger.Info(topic);
    Logger.Info("]: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println("");
}

static String generateTelemetryPayload(const TelemetryData &data)
{
    DynamicJsonDocument doc(256);
    doc["setTemperature"] = data.setTemperature;
    doc["currentTemperature"] = data.currentTemperature;
    doc["currentMode"] = static_cast<int>(data.currentMode);
    doc["currentMotorState"] = static_cast<int>(data.currentMotorState);

    String telemetry_payload = "{}";

    serializeJson(doc, telemetry_payload);

    return telemetry_payload;
}

static void handleGetDataMethod(const char *method_id, char *payload, unsigned int length)
{
    TelemetryData data;
    // Gather telemetry data (this is just a placeholder and should be replaced with actual logic)
    data.setTemperature = 25.0;
    data.currentTemperature = 23.5;

    String telemetryPayload = generateTelemetryPayload(data);

    // Create response topic
    char response_topic[128];
    snprintf(response_topic, sizeof(response_topic), "$iothub/methods/res/200/?$rid=%s", method_id);

    // Send the response
    if (esp_mqtt_client_publish(
            mqtt_client,
            response_topic,
            telemetryPayload.c_str(),
            telemetryPayload.length(),
            MQTT_QOS1,
            DO_NOT_RETAIN_MSG) == 0)
    {
        Logger.Error("Failed publishing Direct Method response");
    }
    else
    {
        Logger.Info("Direct Method response published successfully");
    }
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
        int i, r;

    case MQTT_EVENT_ERROR:
        Logger.Info("MQTT event MQTT_EVENT_ERROR");
        break;
    case MQTT_EVENT_CONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_CONNECTED");

        r = esp_mqtt_client_subscribe(mqtt_client, "$iothub/methods/POST/#", 1);
        if (r == -1)
        {
            Logger.Error("Could not subscribe for Direct Methods.");
        }
        else
        {
            Logger.Info("Subscribed for Direct Methods; message id:" + String(r));
        }

        break;
    case MQTT_EVENT_DISCONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        Logger.Info("MQTT event MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        Logger.Info("MQTT event MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        Logger.Info("MQTT event MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        Logger.Info("MQTT event MQTT_EVENT_DATA");

        for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->topic_len; i++)
        {
            incoming_data[i] = event->topic[i];
        }
        incoming_data[i] = '\0';
        Logger.Info("Topic: " + String(incoming_data));

        AzureIoTManager::requestTelemetrySend();

        for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->data_len; i++)
        {
            incoming_data[i] = event->data[i];
        }
        incoming_data[i] = '\0';
        Logger.Info("Data: " + String(incoming_data));

        if (strstr(event->topic, "$iothub/methods/POST/") != NULL)
        {
            char *method_name = event->topic + strlen("$iothub/methods/POST/");
            char *method_id_start = strstr(method_name, "/?$rid=");
            if (method_id_start != NULL)
            {
                *method_id_start = '\\0'; // Null terminate the method name
                method_id_start += strlen("/?$rid=");

                if (strcmp(method_name, "GetDataMethod") == 0)
                {
                    handleGetDataMethod(method_id_start, event->data, event->data_len);
                }
                else
                {
                    Logger.Info("No Direct Method Found.");
                }
            }
        }

        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        Logger.Info("MQTT event MQTT_EVENT_BEFORE_CONNECT");
        break;
    default:
        Logger.Error("MQTT event UNKNOWN");
        break;
    }

    return ESP_OK;
}

static void initializeIoTHubClient()
{
    az_iot_hub_client_options options = az_iot_hub_client_options_default();
    options.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

    if (az_result_failed(az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)host, strlen(host)),
            az_span_create((uint8_t *)device_id, strlen(device_id)),
            &options)))
    {
        Logger.Error("Failed initializing Azure IoT Hub client");
        return;
    }

    size_t client_id_length;
    if (az_result_failed(az_iot_hub_client_get_client_id(
            &client, mqtt_client_id, sizeof(mqtt_client_id) - 1, &client_id_length)))
    {
        Logger.Error("Failed getting client id");
        return;
    }

    if (az_result_failed(az_iot_hub_client_get_user_name(
            &client, mqtt_username, sizeofarray(mqtt_username), NULL)))
    {
        Logger.Error("Failed to get MQTT clientId, return code");
        return;
    }

    Logger.Info("Client ID: " + String(mqtt_client_id));
    Logger.Info("Username: " + String(mqtt_username));
}

static int initializeMqttClient()
{
#ifndef IOT_CONFIG_USE_X509_CERT
    if (sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES) != 0)
    {
        Logger.Error("Failed generating SAS token");
        return 1;
    }
#endif

    esp_mqtt_client_config_t mqtt_config;
    memset(&mqtt_config, 0, sizeof(mqtt_config));
    mqtt_config.uri = mqtt_broker_uri;
    mqtt_config.port = mqtt_port;
    mqtt_config.client_id = mqtt_client_id;
    mqtt_config.username = mqtt_username;

    mqtt_config.password = (const char *)az_span_ptr(sasToken.Get());
    mqtt_config.keepalive = 30;
    mqtt_config.disable_clean_session = 0;
    mqtt_config.disable_auto_reconnect = false;
    mqtt_config.event_handle = mqtt_event_handler;
    mqtt_config.user_context = NULL;
    mqtt_config.cert_pem = (const char *)ca_pem;

    mqtt_client = esp_mqtt_client_init(&mqtt_config);

    if (mqtt_client == NULL)
    {
        Logger.Error("Failed creating mqtt client");
        return 1;
    }

    esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

    if (start_result != ESP_OK)
    {
        Logger.Error("Could not start mqtt client; error code:" + start_result);
        return 1;
    }
    else
    {
        Logger.Info("MQTT client started");
        return 0;
    }
}

/*
 * @brief           Gets the number of seconds since UNIX epoch until now.
 * @return uint32_t Number of seconds.
 */
static uint32_t getEpochTimeInSecs() { return (uint32_t)time(NULL); }

static void sendTelemetry(const TelemetryData &data)
{
    Logger.Info("Sending telemetry ...");
    if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(
            &client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL)))
    {
        Logger.Error("Failed az_iot_hub_client_telemetry_get_publish_topic");
        return;
    }

    String telemetry_payload = generateTelemetryPayload(data);

    if (esp_mqtt_client_publish(
            mqtt_client,
            telemetry_topic,
            (const char *)telemetry_payload.c_str(),
            telemetry_payload.length(),
            MQTT_QOS1,
            DO_NOT_RETAIN_MSG) == 0)
    {
        Logger.Error("Failed publishing");
    }
    else
    {
        Logger.Info("Message published successfully");
    }
}

AzureIoTManager::AzureIoTManager() {}

AzureIoTManager::~AzureIoTManager() {}

void AzureIoTManager::begin()
{
    connectToWiFi();
    initializeTime();
    initializeIoTHubClient();
    (void)initializeMqttClient();
}

void AzureIoTManager::update(const TelemetryData &data)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connectToWiFi();
    }
    else if (sasToken.IsExpired())
    {
        Logger.Info("SAS token expired; reconnecting with a new one.");
        (void)esp_mqtt_client_destroy(mqtt_client);
        initializeMqttClient();
    }
    else if (sendTelemetryOnRequest)
    {
        sendTelemetry(data);
        AzureIoTManager::sendTelemetryOnRequest = false;
        Logger.Info("Message Sent on Request.");
    }
}
