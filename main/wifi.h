#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "app.h"
#include "settings.h"

class Wifi {
  public:
    Wifi(App*, Settings::Store*);

    void connect();
    void disconnect();

    static void onStart(void *arg, esp_event_base_t base, int32_t id, void *data);
    static void onConnect(void *arg, esp_event_base_t base, int32_t id, void *data);
    static void onDisconnect(void *arg, esp_event_base_t base, int32_t id, void *data);
    static void onIp(void *arg, esp_event_base_t base, int32_t id, void *data);
    static void onIpv6(void *arg, esp_event_base_t base, int32_t id, void *data);

  private:

    App* app;
    Settings::Store* settings;
    esp_netif_t *netif;

    void start();
    static void stop();
    void init();
    void createNetif();
    void registerHandlers();
    void configure();
    void logInfo();
    void waitForIps();

    static bool isOwnNetif(const char *prefix, esp_netif_t *netif);
    static esp_netif_t* netifFromDesc(const char *desc);
};
#endif
