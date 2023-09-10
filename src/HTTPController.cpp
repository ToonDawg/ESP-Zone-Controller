#include "HTTPController.h"

HTTPController::HTTPController(const char* ssid, const char* password, TemperatureController& tempController)
    : _ssid(ssid), _password(password), _tempController(tempController), _server(80) {}

void HTTPController::begin() {
    // Connect to Wi-Fi
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    setupRoutes();
    _server.begin();
}

void HTTPController::loop() {
    // Can be used if there's any continuous monitoring or updates required for the HTTP controller.
}

void HTTPController::end() {
    _server.end();
}

void HTTPController::setupRoutes() {
    // Endpoint to update temperature
    _server.on("/setTemperature", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("temperature", true)) {
            String temperature = request->getParam("temperature", true)->value();
            // Use the _tempController to update the temperature
            _tempController.setTemperature(temperature.toFloat());
            request->send(200, "text/plain", "Temperature updated");
        } else {
            request->send(400, "text/plain", "No temperature specified");
        }
    });

    // Endpoint to fetch current temperature
    _server.on("/getTemperature", HTTP_GET, [this](AsyncWebServerRequest *request) {
        float currentTemperature = _tempController.getCurrentTemperature();
        request->send(200, "text/plain", String(currentTemperature));
    });
}
