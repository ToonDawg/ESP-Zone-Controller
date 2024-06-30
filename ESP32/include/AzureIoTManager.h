#pragma once

#include <Arduino.h>
#include <mqtt_client.h>
#include <az_iot.h>
#include "TemperatureController.h"
#include "AzIoTSasToken.h"

class AzureIoTManager {
public:
    AzureIoTManager(TemperatureController& tempController);
    
    void begin();
    void update();
    bool isConnected() const;

private:
    static constexpr size_t INCOMING_DATA_BUFFER_SIZE = 128;
    static constexpr int SAS_TOKEN_DURATION_IN_MINUTES = 60;

    TemperatureController& tempController;
    esp_mqtt_client_handle_t mqttClient;
    az_iot_hub_client azClient;
    AzIoTSasToken sasToken;

    char incomingData[INCOMING_DATA_BUFFER_SIZE];
    char mqttClientId[128];
    char mqttUsername[128];
    char mqttPassword[200];
    char telemetryTopic[128];

    uint32_t telemetrySendCount;
    unsigned long nextTelemetrySendTime;

    void initializeMqttClient();
    void initializeIoTHubClient();
    void sendTelemetry();
    void handleIncomingMessage(esp_mqtt_event_handle_t event);
    void subscribeToTopics();

    static esp_err_t mqttEventHandlerStatic(esp_mqtt_event_handle_t event);
    esp_err_t mqttEventHandler(esp_mqtt_event_handle_t event);
};