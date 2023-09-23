#ifndef AzureIoTManager_h
#define AzureIoTManager_h

// Standard Include Directives
#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <az_core.h>
#include <az_iot.h>

// Custom Include Directives
#include "AzIoTSasToken.h"
#include "SerialLogger.h"
#include "iot_configs.h"
#include "TemperatureController.h"

// Constants
#define AZURE_SDK_CLIENT_USER_AGENT "c%2F" AZ_SDK_VERSION_STRING "(ard;esp32)"
#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"
#define DO_NOT_RETAIN_MSG 0
#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define PST_TIME_ZONE -8
#define PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF 1
#define INCOMING_DATA_BUFFER_SIZE 128
#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define MQTT_QOS1 1
#define UNIX_TIME_NOV_13_2017 1510592825
#define GMT_OFFSET_SECS (PST_TIME_ZONE * 3600)
#define GMT_OFFSET_SECS_DST ((PST_TIME_ZONE + PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF) * 3600)
#define METHOD_POST_PREFIX "$iothub/methods/POST/"
#define METHOD_RID_PREFIX "/?$rid="

class AzureIoTManager
{
private:
    esp_mqtt_client_handle_t mqtt_client;
    az_iot_hub_client client;
    char incoming_data[INCOMING_DATA_BUFFER_SIZE];
    const char *host = IOT_CONFIG_IOTHUB_FQDN;
    const char *device_id = IOT_CONFIG_DEVICE_ID;
    char mqtt_client_id[128];
    char mqtt_username[128];
    char mqtt_password[200];
    const char *mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
    uint8_t sas_signature_buffer[256];
    const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;
    uint32_t telemetry_send_count = 0;
    char telemetry_topic[128];
    unsigned long next_telemetry_send_time_ms = 0;
    AzIoTSasToken sasToken;
    TemperatureController& tempController;
    esp_mqtt_client_config_t mqtt_config;


public:
    AzureIoTManager(TemperatureController& tempController);
    void begin();
    void update(const TelemetryData &data);
    void sendTelemetry(const TelemetryData &data);
    void sendResponse(char *method_id, String &telemetryPayload);

private:
    void handleGetDataMethod(char *method_id, char *payload, unsigned int length);
    void handleUpdateDataMethod(char *method_id, char *payload, unsigned int length);
    void subscribeToTopic(const char *topic, const char *description);
    static esp_err_t mqtt_event_handler_static(esp_mqtt_event_handle_t event);
    esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
    int initializeMqttClient();
    void initializeIoTHubClient();
    void handleMethod(const esp_mqtt_event_handle_t event);
};

#endif
