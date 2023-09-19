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

class AzureIoTManager {
public:
    AzureIoTManager();
    ~AzureIoTManager();
    void begin();
    void update(const TelemetryData& data); 
    static void requestTelemetrySend() { sendTelemetryOnRequest = true; }
private:
    static bool sendTelemetryOnRequest; //RIP for testing
};

#endif
