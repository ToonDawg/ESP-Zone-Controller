#include "OTAUpdater.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

OTAUpdater::OTAUpdater(const char* base_url, const char* device_name)
    : _base_url(base_url), _device_name(device_name) {}

String OTAUpdater::getLatestVersion()
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Cannot check for latest version.");
        return "";
    }

    String versions_url = String(_base_url) + "/" + _device_name + "/versions.json";
    Serial.println("Fetching latest version from: " + versions_url);
    String payload = httpGet(versions_url);
    
    Serial.println("Received payload: " + payload);
    
    payload.trim();
    payload.replace("\"", "");
    
    if (payload.length() > 0) {
        Serial.println("Latest version found: " + payload);
        return payload;
    }
    
    Serial.println("Failed to parse latest version");
    return "";
}

void OTAUpdater::updateUrls(const String& version)
{
    _firmware_url = String(_base_url) + "/" + _device_name + "/" + version + "/firmware.bin";
    _metadata_url = String(_base_url) + "/" + _device_name + "/" + version + "/metadata.json";
    Serial.println("Updated firmware URL: " + _firmware_url);
    Serial.println("Updated metadata URL: " + _metadata_url);
}

bool OTAUpdater::checkForUpdates()
{
    Serial.println("Checking for updates...");
    String latestVersion = getLatestVersion();
    if (latestVersion.isEmpty()) {
        Serial.println("No update available.");
        return false;
    }

    updateUrls(latestVersion);
    String metadata = httpGet(_metadata_url);
    
    if (!metadata.isEmpty()) {
        Serial.println("Update check successful");
        Serial.println("Metadata content: " + metadata);
        return true;
    }
    
    Serial.println("Failed to fetch metadata. No update available.");
    return false;
}

void OTAUpdater::performUpdate()
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Cannot perform update.");
        return;
    }

    Serial.println("Starting update process...");
    HTTPClient http;
    http.begin(_firmware_url);
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        Serial.println("Error on HTTP request. Code: " + String(httpCode));
        Serial.println("Update failed.");
        http.end();
        return;
    }

    if (downloadUpdate(http) && finalizeUpdate()) {
        Serial.println("Update successful. Restarting device...");
        ESP.restart();
    } else {
        Serial.println("Update process failed.");
    }

    http.end();
}

bool OTAUpdater::downloadUpdate(HTTPClient &http)
{
    int contentLength = http.getSize();
    if (contentLength <= 0) {
        Serial.println("Invalid content length. Update aborted.");
        return false;
    }

    Serial.println("Update size: " + String(contentLength) + " bytes");

    if (!Update.begin(contentLength)) {
        Serial.println("Not enough space to begin OTA. Update aborted.");
        return false;
    }

    Serial.println("Downloading update...");
    size_t written = Update.writeStream(*http.getStreamPtr());
    if (written != contentLength) {
        Serial.println("Written only: " + String(written) + "/" + String(contentLength) + ". Update failed.");
        return false;
    }

    Serial.println("Download successful");
    return true;
}

bool OTAUpdater::finalizeUpdate()
{
    Serial.println("Finalizing update...");
    if (!Update.end()) {
        Serial.println("Error occurred. Error #: " + String(Update.getError()));
        return false;
    }

    if (!Update.isFinished()) {
        Serial.println("Update not finished. Something went wrong!");
        return false;
    }

    Serial.println("Update successfully completed. Ready to reboot.");
    return true;
}

String OTAUpdater::httpGet(const String& url)
{
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    String payload = "";
    if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println("HTTP GET successful: " + url);
    } else {
        Serial.println("HTTP GET failed. URL: " + url + ", Code: " + String(httpCode));
    }

    http.end();
    return payload;
}