#include "app.h"

static const char *TAG = "[main]";

App app;
Events events;
Settings::Store settings;
Sensor sensor(&app, &settings);
Heating heating(&app, &settings, &sensor);
Display display(&app, &settings, &heating, &sensor);
Logger logger(&app, &settings);
Metrics metrics(&app, &settings, &heating);
Ota ota(&app, &settings);
Sntp sntp;
Wifi wifi(&app, &settings);

void updateDisplay(void *, esp_event_base_t base, int32_t id, void* data) {
  display.update(id);
}

void App::start() {
  ESP_LOGI(TAG, "Starting up ...");

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set("esp-x509-crt-bundle", ESP_LOG_WARN);
  esp_event_loop_create_default();
  esp_register_shutdown_handler(shutdown);
  nvs_flash_init();
  logSystemInfo();

  settings.init();
  events.init();
  display.init();
  sensor.init();
  heating.init();

  progress(BOOTING_WIFI);
  wifi.connect();

  progress(BOOTING_SNTP);
  sntp.init();
  logger.init();
  ota.init();
  metrics.init();

  progress(RUNNING);
}

void App::shutdown() {
  ESP_LOGI(TAG, "Shutdown ...");
  heating.turnOff();
  logger.stop();
}

void App::listen(void* listener, uint8_t event) {
  events.listen((int32_t) event, updateDisplay); // no idea how to use listener
}

void App::publish(uint8_t event) {
  events.post(event);
}

void App::progress(uint8_t s) {
  state = s;
  events.post(BOOT_PROGRESS);
}

bool App::isBooting() {
  return state != RUNNING;
}

bool App::hasErrors() {
  return errors > 0;
}

bool App::isError(uint8_t error) {
  return (errors & error) != 0;
}

void App::addError(uint8_t error) {
  errors |= error;
}

void App::removeError(uint8_t error) {
  errors &= ~error;
}
