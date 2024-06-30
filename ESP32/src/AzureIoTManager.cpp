// #include "AzureIoTManager.h"
// #include "iot_configs.h"
// #include "SerialLogger.h"
// #include <az_core.h>
// #include <WiFi.h>

// // Constants moved from header
// #define AZURE_SDK_CLIENT_USER_AGENT "c%2F" AZ_SDK_VERSION_STRING "(ard;esp32)"
// #define NTP_SERVERS "pool.ntp.org", "time.nist.gov"
// #define MQTT_QOS1 1
// #define METHOD_POST_PREFIX "$iothub/methods/POST/"
// #define METHOD_RID_PREFIX "/?$rid="

// AzureIoTManager::AzureIoTManager(TemperatureController& controller)
//     : tempController(controller), telemetrySendCount(0), nextTelemetrySendTime(0),
//       sasToken(nullptr, az_span_create_from_str((uint8_t*)IOT_CONFIG_DEVICE_KEY, strlen(IOT_CONFIG_DEVICE_KEY)),
//                az_span_create_from_str((uint8_t*)sas_signature_buffer, sizeof(sas_signature_buffer)),
//                az_span_create_from_str((uint8_t*)mqttPassword, sizeof(mqttPassword)))
// {
// }

// void AzureIoTManager::begin() {
//     SerialLogger::getInstance()->info("Initializing Azure IoT Manager...");

//     initializeIoTHubClient();
//     initializeMqttClient();

//     SerialLogger::getInstance()->info("Azure IoT Manager initialized");
// }

// void AzureIoTManager::update() {
//     unsigned long currentTime = millis();

//     if (currentTime > nextTelemetrySendTime) {
//         sendTelemetry();
//         nextTelemetrySendTime = currentTime + 10000; // Send telemetry every 10 seconds
//     }
// }

// bool AzureIoTManager::isConnected() const {
//     return esp_mqtt_client_is_connected(mqttClient);
// }

// void AzureIoTManager::initializeIoTHubClient() {
//     az_iot_hub_client_options options = az_iot_hub_client_options_default();
//     options.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

//     if (az_result_failed(az_iot_hub_client_init(
//             &azClient,
//             az_span_create((uint8_t*)IOT_CONFIG_IOTHUB_FQDN, strlen(IOT_CONFIG_IOTHUB_FQDN)),
//             az_span_create((uint8_t*)IOT_CONFIG_DEVICE_ID, strlen(IOT_CONFIG_DEVICE_ID)),
//             &options))) {
//         SerialLogger::getInstance()->error("Failed to initialize Azure IoT Hub client");
//         return;
//     }

//     size_t client_id_length;
//     if (az_result_failed(az_iot_hub_client_get_client_id(
//             &azClient, mqttClientId, sizeof(mqttClientId) - 1, &client_id_length))) {
//         SerialLogger::getInstance()->error("Failed to get MQTT client id");
//         return;
//     }

//     size_t username_length;
//     if (az_result_failed(az_iot_hub_client_get_user_name(
//             &azClient, mqttUsername, sizeof(mqttUsername) - 1, &username_length))) {
//         SerialLogger::getInstance()->error("Failed to get MQTT username");
//         return;
//     }

//     if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(
//             &azClient, NULL, telemetryTopic, sizeof(telemetryTopic), NULL))) {
//         SerialLogger::getInstance()->error("Failed to get telemetry topic");
//         return;
//     }
// }

// void AzureIoTManager::initializeMqttClient() {
//     esp_mqtt_client_config_t mqtt_config;
//     memset(&mqtt_config, 0, sizeof(mqtt_config));
//     mqtt_config.uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
//     mqtt_config.port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;
//     mqtt_config.client_id = mqttClientId;
//     mqtt_config.username = mqttUsername;
//     mqtt_config.password = (const char*)az_span_ptr(sasToken.Get());
//     mqtt_config.keepalive = 30;
//     mqtt_config.disable_clean_session = 0;
//     mqtt_config.disable_auto_reconnect = false;
//     mqtt_config.event_handle = mqttEventHandlerStatic;
//     mqtt_config.user_context = this;

//     mqttClient = esp_mqtt_client_init(&mqtt_config);
//     if (mqttClient == NULL) {
//         SerialLogger::getInstance()->error("Failed to initialize MQTT client");
//         return;
//     }

//     esp_mqtt_client_start(mqttClient);
// }

// void AzureIoTManager::sendTelemetry() {
//     az_span telemetry = AZ_SPAN_FROM_BUFFER(incomingData);

//     TemperatureController::Status status = tempController.getStatus();
    
//     snprintf(incomingData, sizeof(incomingData),
//              "{ \"msgCount\": %d, \"setTemp\": %.2f, \"currentTemp\": %.2f }",
//              telemetrySendCount++, status.setTemperature, status.currentTemperature);

//     if (esp_mqtt_client_publish(
//             mqttClient,
//             telemetryTopic,
//             (const char*)az_span_ptr(telemetry),
//             az_span_size(telemetry),
//             MQTT_QOS1,
//             DO_NOT_RETAIN_MSG) == 0) {
//         SerialLogger::getInstance()->error("Failed to publish telemetry");
//     }
// }

// void AzureIoTManager::handleIncomingMessage(esp_mqtt_event_handle_t event) {
//     az_span topic_span = az_span_create((uint8_t*)event->topic, event->topic_len);
//     az_span message_span = az_span_create((uint8_t*)event->data, event->data_len);

//     az_iot_hub_client_method_request method_request;
//     if (az_result_succeeded(az_iot_hub_client_methods_parse_received_topic(&azClient, topic_span, &method_request))) {
//         SerialLogger::getInstance()->info("Received method request");
//         // Handle method request
//     } else {
//         SerialLogger::getInstance()->info("Received message on topic: %.*s", event->topic_len, event->topic);
//         SerialLogger::getInstance()->info("Message: %.*s", event->data_len, event->data);
//     }
// }

// void AzureIoTManager::subscribeToTopics() {
//     char methods_topic[128];
//     if (az_result_succeeded(az_iot_hub_client_methods_subscribe_topic_filter(
//             &azClient, methods_topic, sizeof(methods_topic), NULL))) {
//         esp_mqtt_client_subscribe(mqttClient, methods_topic, 1);
//     }
// }

// esp_err_t AzureIoTManager::mqttEventHandlerStatic(esp_mqtt_event_handle_t event) {
//     AzureIoTManager* manager = (AzureIoTManager*)event->user_context;
//     return manager->mqttEventHandler(event);
// }

// esp_err_t AzureIoTManager::mqttEventHandler(esp_mqtt_event_handle_t event) {
//     switch (event->event_id) {
//         case MQTT_EVENT_CONNECTED:
//             SerialLogger::getInstance()->info("MQTT client connected");
//             subscribeToTopics();
//             break;
//         case MQTT_EVENT_DISCONNECTED:
//             SerialLogger::getInstance()->info("MQTT client disconnected");
//             break;
//         case MQTT_EVENT_SUBSCRIBED:
//             SerialLogger::getInstance()->info("MQTT client subscribed to topic");
//             break;
//         case MQTT_EVENT_DATA:
//             handleIncomingMessage(event);
//             break;
//         default:
//             SerialLogger::getInstance()->info("MQTT client event: %d", event->event_id);
//             break;
//     }
//     return ESP_OK;
// }