#ifndef DISPLAY_H
#define DISPLAY_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <u8g2.h>
#include "u8g2_hal.h"
#include "app.h"
#include "events.h"
#include "heating.h"
#include "sensor.h"
#include "settings.h"

class Display {
  public:

    Display(App*, Settings::Store*, Heating*, Sensor*);
    void init();
    void update(int32_t id);
    void clear();

  private:

    u8g2_t u8g2;
    App* app;
    Heating* heating;
    Settings::Store* settings;
    Sensor* sensor;

    void drawBootScreen();
    void drawTempScreen();
    char* formatTemp(float);
    char* formatHumid(float);
    char* formatErrors(int);
};
#endif
