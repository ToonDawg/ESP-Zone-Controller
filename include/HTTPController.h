#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "TemperatureController.h"  // Assuming you have a TemperatureController class

class HTTPController {
public:
    HTTPController(const char* ssid, const char* password, TemperatureController& tempController);
    void begin();
    void loop();
    void end();

private:
    const char* _ssid;
    const char* _password;
    TemperatureController& _tempController;
    AsyncWebServer _server;
    void setupRoutes();
};

#endif // HTTP_CONTROLLER_H
