#ifndef APP_H
#define APP_H

class App;
extern App app;

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "display.h"
#include "events.h"
#include "heating.h"
#include "helper.h"
#include "logger.h"
#include "metrics.h"
#include "ota.h"
#include "relay.h"
#include "sensor.h"
#include "sntp.h"
#include "settings.h"
#include "wifi.h"

enum {
  BOOTING_INIT,
  BOOTING_WIFI,
  BOOTING_SNTP,
  RUNNING
};

enum {
  ERR_SENSOR_READ = 1,
  ERR_HTTP_METRICS = 2,
  ERR_HTTP_OTA = 4,
  ERR_UDP_LOG = 8,
};

class App {
  public:
    uint8_t state = BOOTING_INIT;
    uint8_t errors = 0;

    void start();
    static void shutdown();
    void listen(void* listener, uint8_t event);
    void publish(uint8_t event);
    void progress(uint8_t);

    bool isBooting();
    bool hasErrors();
    bool isError(uint8_t);
    void addError(uint8_t);
    void removeError(uint8_t);
};
#endif
