#ifndef OTA_H
#define OTA_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "app.h"
#include "settings.h"

class Ota;
extern Ota ota;

class Ota {
  public:
    Ota(App*, Settings::Store*);
    void init();
    void run();

  private:
    App* app;
    Settings::Store* settings;
    esp_https_ota_handle_t handle;
    bool errored;

    bool start();
    bool validate();
    void read();
    void finish();
};
#endif
