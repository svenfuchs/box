#include "relay.h"

#define RELAY_PIN GPIO_NUM_26
#define LOW 0
#define HIGH 1

void Relay::init() {
  gpio_set_direction(RELAY_PIN, GPIO_MODE_OUTPUT);
}

void Relay::turnOn() {
  set(HIGH);
}

void Relay::turnOff() {
  set(LOW);
}

void Relay::set(int value) {
  gpio_set_level(RELAY_PIN, (u_int32_t) value);
}
