#ifndef SENSOR_H
#define SENSOR_H

class Sensor;

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht.h"
#include "esp_log.h"
#include "app.h"
#include "helper.h"
#include "settings.h"

class Sensor {
  public:

    float temperature = 0;
    float humidity = 0;

    Sensor(App*, Settings::Store*);
    void init();
    void run();

  private:

    static const dht_sensor_type_t type = DHT_TYPE_AM2301;
    static const gpio_num_t gpio = GPIO_NUM_32;
    int failures = 0;
    bool failed = false;

    App* app;
    Settings::Store* settings;

    void update();
    esp_err_t read();
    void success();
    void failure();
    void info();
    void delay();
};
#endif
