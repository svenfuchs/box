#ifndef RELAY_H
#define RELAY_H
#include "driver/gpio.h"

class Relay {
  public:
    void init();
    void turnOn();
    void turnOff();

  private:
    void set(int value);
};
#endif
