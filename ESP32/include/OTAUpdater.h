#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "Settings.h"

class OTAUpdater
{
public:
    OTAUpdater(const char *base_url, const char *device_name, Settings &settings);
    void performUpdate();
    String getLatestVersion();
    String getStoredLatestVersion() const { return _latestVersion; }
    bool isNewerVersion(const String &currentVersion, const String &latestVersion);

private:
    const char *_base_url;
    const char *_device_name;
    String _firmware_url;
    String _metadata_url;
    String _latestVersion;
    Settings &_settings;
    void updateUrls(const String &version);
    bool isUpdateAvailable();
    bool downloadUpdate(HTTPClient &http);
    bool finalizeUpdate();
    String httpGet(const String &url);
    int compareVersions(const String &v1, const String &v2);
};

#endif // OTA_UPDATER_H