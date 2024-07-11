#include "OTAUpdater.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

OTAUpdater::OTAUpdater(const char* firmware_url, const char* metadata_url)
    : _firmware_url(firmware_url), _metadata_url(metadata_url) {}

void OTAUpdater::begin() {
    // Initialize any necessary components
}

bool OTAUpdater::checkForUpdates() {
    HTTPClient https;
    https.begin(_metadata_url);
    int httpCode = https.GET();
    
    bool updateAvailable = (httpCode == HTTP_CODE_OK);
    https.end();
    return updateAvailable;
}

bool OTAUpdater::downloadUpdate(HTTPClient& https) {
    int contentLength = https.getSize();
    if (contentLength <= 0) {
        Serial.println("Invalid content length");
        return false;
    }

    if (!Update.begin(contentLength)) {
        Serial.println("Not enough space to begin OTA");
        return false;
    }

    size_t written = Update.writeStream(https.getStream());
    if (written != contentLength) {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
        return false;
    }

    Serial.println("Download successful");
    return true;
}

bool OTAUpdater::finalizeUpdate() {
    if (!Update.end()) {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        return false;
    }

    if (!Update.isFinished()) {
        Serial.println("Update not finished? Something went wrong!");
        return false;
    }

    Serial.println("Update successfully completed. Rebooting.");
    return true;
}

void OTAUpdater::performUpdate() {
    HTTPClient https;
    https.begin(_firmware_url);
    int httpCode = https.GET();
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.println("Error on HTTP request");
        https.end();
        return;
    }

    if (!downloadUpdate(https)) {
        https.end();
        return;
    }

    https.end();

    if (finalizeUpdate()) {
        ESP.restart();
    }
}