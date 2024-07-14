#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include <HTTPClient.h>

class OTAUpdater {
public:
    OTAUpdater(const char* base_url, const char* device_name);
    bool checkForUpdates();
    void performUpdate();

private:
    const char* _base_url;
    const char* _device_name;
    String _firmware_url;
    String _metadata_url;

    String getLatestVersion();
    void updateUrls(const String& version);
    bool downloadUpdate(HTTPClient& http);
    bool finalizeUpdate();
    String httpGet(const String& url);
};

#endif // OTA_UPDATER_H