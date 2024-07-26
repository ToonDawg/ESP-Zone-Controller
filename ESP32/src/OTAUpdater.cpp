#include "OTAUpdater.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <vector>

OTAUpdater::OTAUpdater(const char *base_url, const char *device_name, Settings &settings)
    : _base_url(base_url), _device_name(device_name), _settings(settings) {}

String OTAUpdater::getLatestVersion()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected. Cannot check for latest version.");
        return "";
    }

    String versions_url = String(_base_url) + "/" + _device_name + "/versions.json";
    Serial.println("Fetching latest version from: " + versions_url);
    String payload = httpGet(versions_url);

    Serial.println("Received payload: " + payload);

    payload.trim();
    payload.replace("\"", "");

    if (payload.length() > 0)
    {
        Serial.println(payload);
        _latestVersion = payload;
        updateUrls(_latestVersion);
        return payload;
    }

    Serial.println("Failed to parse latest version");
    return "";
}

void OTAUpdater::updateUrls(const String &version)
{
    _firmware_url = String(_base_url) + "/" + _device_name + "/" + version + "/firmware.bin";
    _metadata_url = String(_base_url) + "/" + _device_name + "/" + version + "/metadata.json";
    Serial.println("Updated firmware URL: " + _firmware_url);
    Serial.println("Updated metadata URL: " + _metadata_url);
}

bool OTAUpdater::isUpdateAvailable()
{
    Serial.println("Checking for updates...");
    String currentVersion = _settings.getCurrentSoftwareVersion();
    String latestVersion = getLatestVersion();
    if (latestVersion.isEmpty())
    {
        Serial.println("Failed to fetch latest version.");
        return false;
    }

    Serial.println("Current version: " + currentVersion);
    Serial.println("Latest version: " + latestVersion);

    if (!isNewerVersion(currentVersion, latestVersion))
    {
        Serial.println("No newer version available.");
        return false;
    }

    String metadata = httpGet(_metadata_url);

    if (!metadata.isEmpty())
    {
        Serial.println("Update check successful");
        Serial.println("Metadata content: " + metadata);
        return true;
    }

    Serial.println("Failed to fetch metadata. No update available.");
    return false;
}

bool OTAUpdater::isNewerVersion(const String &currentVersion, const String &latestVersion)
{
    return compareVersions(currentVersion, latestVersion) < 0;
}

int OTAUpdater::compareVersions(const String &v1, const String &v2)
{
    std::vector<int> parts1, parts2;

    // Split v1 into parts
    int start = 0;
    int end = v1.indexOf('.');
    while (end != -1)
    {
        parts1.push_back(v1.substring(start, end).toInt());
        start = end + 1;
        end = v1.indexOf('.', start);
    }
    parts1.push_back(v1.substring(start).toInt());

    // Split v2 into parts
    start = 0;
    end = v2.indexOf('.');
    while (end != -1)
    {
        parts2.push_back(v2.substring(start, end).toInt());
        start = end + 1;
        end = v2.indexOf('.', start);
    }
    parts2.push_back(v2.substring(start).toInt());

    // Compare parts
    for (size_t i = 0; i < std::max(parts1.size(), parts2.size()); ++i)
    {
        int n1 = (i < parts1.size()) ? parts1[i] : 0;
        int n2 = (i < parts2.size()) ? parts2[i] : 0;
        if (n1 < n2)
            return -1;
        if (n1 > n2)
            return 1;
    }

    return 0;
}

// OTAUpdater.cpp

void OTAUpdater::performUpdate()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected. Cannot perform update.");
        return;
    }

    if (!isUpdateAvailable())
    {
        Serial.println("No update available.");
        return;
    }

    Serial.println("Starting update process...");
    Serial.println("Firmware URL: " + _firmware_url);  // Log the URL for debugging

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
    Serial.println("Connecting to update server...");
    if (!http.begin(_firmware_url))
    {
        Serial.println("Failed to connect to the update server.");
        return;
    }

    Serial.println("Sending HTTP GET request...");
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.println("Error on HTTP request. Code: " + String(httpCode));
        Serial.println("Error description: " + http.errorToString(httpCode));
        Serial.println("Update failed.");
        http.end();
        return;
    }

    Serial.println("HTTP request successful. Starting download...");
    if (downloadUpdate(http) && finalizeUpdate())
    {
        Serial.println("Update successful. Updating version and restarting device...");
        http.end();
        ESP.restart();
    }
    else
    {
        Serial.println("Update process failed.");
    }

    http.end();
}

bool OTAUpdater::downloadUpdate(HTTPClient &http)
{
    int contentLength = http.getSize();
    if (contentLength <= 0)
    {
        Serial.println("Invalid content length. Update aborted.");
        return false;
    }

    Serial.println("Update size: " + String(contentLength) + " bytes");

    if (!Update.begin(contentLength))
    {
        Serial.println("Not enough space to begin OTA. Update aborted.");
        return false;
    }

    Serial.println("Downloading update...");
    size_t written = Update.writeStream(*http.getStreamPtr());
    if (written != contentLength)
    {
        Serial.println("Written only: " + String(written) + "/" + String(contentLength) + ". Update failed.");
        return false;
    }

    Serial.println("Download successful");
    return true;
}

bool OTAUpdater::finalizeUpdate()
{
    Serial.println("Finalizing update...");
    if (!Update.end())
    {
        Serial.println("Error occurred. Error #: " + String(Update.getError()));
        return false;
    }

    if (!Update.isFinished())
    {
        Serial.println("Update not finished. Something went wrong!");
        return false;
    }
    _settings.setCurrentSoftwareVersion(_latestVersion);

    Serial.println("Update successfully completed. Ready to reboot.");
    return true;
}

String OTAUpdater::httpGet(const String &url)
{
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    String payload = "";
    if (httpCode == HTTP_CODE_OK)
    {
        payload = http.getString();
        Serial.println("HTTP GET successful: " + url);
    }
    else
    {
        Serial.println("HTTP GET failed. URL: " + url + ", Code: " + String(httpCode));
    }

    http.end();
    return payload;
}