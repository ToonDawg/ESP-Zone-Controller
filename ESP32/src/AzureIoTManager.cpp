#include "AzureIoTManager.h"
#include "iot_configs.h"
#include <azure_ca.h>
#include "ArduinoJson.h"

char *extractRid(const char *input, char *buffer, size_t bufferLen);

void connectToWiFi()
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

void initializeTime()
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

String generateTelemetryPayload(const TelemetryData &data)
{
    DynamicJsonDocument doc(256);
    doc["deviceId"] = "FirstTestDevice";
    doc["setTemperature"] = data.setTemperature;
    doc["currentTemperature"] = data.currentTemperature;
    doc["currentMode"] = static_cast<int>(data.currentMode);
    doc["currentMotorState"] = static_cast<int>(data.currentMotorState);

    String telemetry_payload = "";

    serializeJson(doc, telemetry_payload);

    return telemetry_payload;
}

void logTopicAndData(const esp_mqtt_event_handle_t event)
{
    Logger.Info("Topic: " + String(event->topic));
    Logger.Info("Data: " + String(event->data));
}

char *extractRid(const char *input, char *buffer, size_t bufferLen)
{
    String strInput = String(input);

    int idStartPos = strInput.indexOf("/?$rid=");
    if (idStartPos == -1)
    {
        Logger.Info("Could not find /?$rid= in the topic.");
        buffer[0] = '\0';
        return buffer;
    }

    idStartPos += strlen("/?$rid=");

    int idEndPos = idStartPos;
    while (idEndPos < strInput.length() && isalnum(strInput[idEndPos]) && !strInput.substring(idEndPos, idEndPos + 4).equals("null"))
    {
        idEndPos++;
    }

    String result = strInput.substring(idStartPos, idEndPos);
    result.toCharArray(buffer, bufferLen);
    if (result.length() > 0)
    {
        Logger.Info("Successfully extracted method ID: " + result);
    }
    else
    {
        Logger.Info("Failed to extract a method ID from the topic.");
    }
    return buffer;
}

/*
 * @brief           Gets the number of seconds since UNIX epoch until now.
 * @return uint32_t Number of seconds.
 */
uint32_t getEpochTimeInSecs() { return (uint32_t)time(NULL); }

AzureIoTManager::AzureIoTManager(TemperatureController &tController) : tempController(tController),
                                                                       sasToken(
                                                                           &client,
                                                                           AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_KEY),
                                                                           AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
                                                                           AZ_SPAN_FROM_BUFFER(mqtt_password)) {}

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
}

void AzureIoTManager::subscribeToTopic(const char *topic, const char *description)
{
    int r = esp_mqtt_client_subscribe(mqtt_client, topic, 1);
    if (r == -1)
    {
        Logger.Error("Could not subscribe for " + String(description));
    }
    else
    {
        Logger.Info("Subscribed for " + String(description) + "; message id:" + String(r));
    }
}

esp_err_t AzureIoTManager::mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_ERROR:
        Logger.Info("MQTT event MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS)
        {
            Logger.Error("Last error code reported from esp-tls: " + String(event->error_handle->esp_tls_last_esp_err));
            Logger.Error("Last tls stack error number: " + String(event->error_handle->esp_tls_stack_err));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            Logger.Error("Connection refused error: " + String(event->error_handle->connect_return_code));
        }
        break;

    case MQTT_EVENT_CONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_CONNECTED");
        subscribeToTopic(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, "cloud-to-device messages");
        subscribeToTopic("$iothub/methods/POST/#", "Direct Methods");
        break;
    case MQTT_EVENT_DISCONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_DISCONNECTED");
        if (sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES) != 0)
        {
            Logger.Error("Failed generating SAS token");
            return 1;
        } else {
            Logger.Info("Generated new SAS token. Expires at: " + String(sasToken.GetExpirationUnixTime()));
            mqtt_config.password = (const char *)az_span_ptr(sasToken.Get());
        }
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
        logTopicAndData(event);
        if (strstr(event->topic, METHOD_POST_PREFIX))
        {
            handleMethod(event);
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

void AzureIoTManager::initializeIoTHubClient()
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

int AzureIoTManager::initializeMqttClient()
{
    if (sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES) != 0)
    {
        Logger.Error("Failed generating SAS token");
        return 1;
    }
    
    mqtt_config.uri = mqtt_broker_uri;
    mqtt_config.port = mqtt_port;
    mqtt_config.client_id = mqtt_client_id;
    mqtt_config.username = mqtt_username;
    mqtt_config.password = (const char *)az_span_ptr(sasToken.Get());
    mqtt_config.keepalive = 30;
    mqtt_config.disable_clean_session = 0;
    mqtt_config.disable_auto_reconnect = false;
    mqtt_config.event_handle = mqtt_event_handler_static;
    mqtt_config.user_context = this;
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

void AzureIoTManager::handleMethod(const esp_mqtt_event_handle_t event)
{
    char methodIdBuffer[50];
    char *methodId = extractRid(event->topic, methodIdBuffer, sizeof(methodIdBuffer));
    if (!*methodId)
    {
        Logger.Info("Failed to extract a method ID from the topic.");
        return;
    }

    Logger.Info("Extracted method ID: " + String(methodId));

    char *method_name_end = strstr(event->topic, METHOD_RID_PREFIX);
    if (method_name_end)
    {
        *method_name_end = '\0';
    }

    char *method_name = event->topic + strlen(METHOD_POST_PREFIX);
    Logger.Info("Extracted method name: " + String(method_name));

    if (strcmp(method_name, "GetDataMethod") == 0)
    {
        handleGetDataMethod(methodId, event->data, event->data_len);
    }
    else if (strcmp(method_name, "UpdateDataMethod") == 0){
        Logger.Info("Method Found: " + String(method_name));
        handleUpdateDataMethod(methodId, event->data, event->data_len);

    }
    else
    {
        Logger.Info("No matching Direct Method Found for method name: " + String(method_name));
    }
}


void AzureIoTManager::handleGetDataMethod(char *method_id, char *payload, unsigned int length)
{
    char tempMethodId[strlen(method_id) + 1];
    strcpy(tempMethodId, method_id);
    tempMethodId[strlen(method_id)] = '\0';
    Logger.Info("Handling Direct Method with Method ID: " + String(tempMethodId));

    if (length > 0)
    {
        char tempPayload[length + 1];
        strncpy(tempPayload, payload, length);
        tempPayload[length] = '\0';
        Logger.Info("Received Payload: " + String(tempPayload));
    }
    else
    {
        Logger.Info("No Payload received with Direct Method");
    }

    String telemetryPayload = generateTelemetryPayload(tempController.getTelemetryData());
    Logger.Info("Generated Telemetry Payload: " + telemetryPayload);
    
    sendResponse(method_id, telemetryPayload);
}

void AzureIoTManager::sendResponse(char *method_id, String &telemetryPayload)
{
    char response_topic[128];
    snprintf(response_topic, sizeof(response_topic), "$iothub/methods/res/200/?$rid=%s", method_id);

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

void AzureIoTManager::handleUpdateDataMethod(char *method_id, char *payload, unsigned int length){
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.println("Failed to parse JSON");
        return;
    }

    if (doc.containsKey("setTemperature")) {
        float setTemp = doc["setTemperature"].as<float>();
        tempController.setSetTemperature(setTemp);
    }

    if (doc.containsKey("currentMode")) {
        Mode mode = static_cast<Mode>(doc["currentMode"].as<int>());
        tempController.setMode(mode);
    }

    if (doc.containsKey("currentMotorState")) {
        MotorState motorState = static_cast<MotorState>(doc["currentMotorState"].as<int>());
        tempController.setMotorState(motorState);
    }

    String telemetryPayload = generateTelemetryPayload(tempController.getTelemetryData());

    sendResponse(method_id, telemetryPayload);

}


void AzureIoTManager::sendTelemetry(const TelemetryData &data)
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

esp_err_t AzureIoTManager::mqtt_event_handler_static(esp_mqtt_event_handle_t event)
{
    AzureIoTManager *instance = (AzureIoTManager *)event->user_context;
    return instance->mqtt_event_handler(event);
}