#ifndef HEATING_H
#define HEATING_H

class Heating;

#include <time.h>
#include <math.h>
#include "esp_log.h"
#include "app.h"
#include "helper.h"
#include "sensor.h"
#include "relay.h"
#include "settings.h"

class Heating {
  public:

    bool active;
    float currentTemp;
    float targetTemp;

    Heating(App* app, Settings::Store*, Sensor*);
    void init();
    void run();
    void turnOn();
    void turnOff();

  private:

    App* app;
    Relay* relay;
    Sensor* sensor;
    Settings::Store* settings;

    int changed;
    int duration;
    int pause;

    void read();
    void update();
    void change(bool);
    void delay();
    bool isValid();
    bool wantsHeat();
    bool canHeat();
    bool needsPause();
    bool hasPaused();
    void logChange(const char*);
    void info();
    int now();
};
#endif
