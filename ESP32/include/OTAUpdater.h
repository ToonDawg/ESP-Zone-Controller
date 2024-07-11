#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include <HTTPClient.h>

class OTAUpdater {
public:
    OTAUpdater(const char* firmware_url, const char* metadata_url);
    void begin();
    bool checkForUpdates();
    void performUpdate();

private:
    const char* _firmware_url;
    const char* _metadata_url;
    
    bool downloadUpdate(HTTPClient& http);
    bool finalizeUpdate();
};

#endif // OTA_UPDATER_H