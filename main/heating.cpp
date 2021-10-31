#include "heating.h"

static const char* TAG = "[heat]";

Heating::Heating(App* a, Settings::Store* st, Sensor* sn) {
  active = false;
  changed = 0;
  app = a;
  relay = new Relay;
  sensor = sn;
  settings = st;
}

void Heating::init() {
  relay->init();
  // read();
  xTaskCreate([](void*) { heating.run(); }, "heating", 1024 * 8, NULL, 5, NULL);
}

void Heating::run() {
  for(;;) {
    read();
    update();
    info();
    delay();
  }
}

void Heating::update() {
  if(isValid() && wantsHeat() && canHeat()) {
    turnOn();
  } else {
    turnOff();
  }
}

void Heating::read() {
  currentTemp = sensor->temperature;
  targetTemp = settings->getHeatingTargetTemp();
  duration = settings->getHeatingMaxDuration();
  pause = settings->getHeatingMinPause();
}

void Heating::delay() {
  vTaskDelay(settings->getHeatingUpdateInterval() / portTICK_PERIOD_MS);
}

bool Heating::isValid() {
  return currentTemp > 10;
}

bool Heating::wantsHeat() {
  return !isnan(currentTemp) && currentTemp < targetTemp;
}

bool Heating::canHeat() {
  return (active && !needsPause()) || (!active && hasPaused());
}

bool Heating::needsPause() {
  return now() - changed >= duration;
}

bool Heating::hasPaused() {
  return now() - changed >= pause;
}

void Heating::turnOn() {
  if(!active) change(true);
}

void Heating::turnOff() {
  if(active) change(false);
}

void Heating::change(bool active_) {
  active = active_;
  active ? relay->turnOn() : relay->turnOff();
  changed = now();
  app->publish(HEATING_UPDATED);
  logChange(active ? "ON" : "Off");
}

void Heating::info() {
  const char* current = ftoa(currentTemp);
  const char* target = ftoa(targetTemp);
  esp_log_level_t level = isValid() ? ESP_LOG_INFO : ESP_LOG_ERROR;
  ESP_LOG_LEVEL(level, TAG, "Heating updated: [%i:%i] %s/%s %s", duration, pause, current, target, active ? "+" : "-");
  delete current;
  delete target;
}

void Heating::logChange(const char* status) {
  ESP_LOGW(TAG, "Heating %s", status);
}

int Heating::now() {
  return time(NULL);
}
