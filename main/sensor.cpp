#include "sensor.h"

static const char *TAG = "[sens]";

Sensor::Sensor(App* a, Settings::Store* s) {
  app = a;
  settings = s;
}

void Sensor::init() {
  gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
  // update();
  xTaskCreate([](void*) { sensor.run(); }, "sensor", 1024 * 8, NULL, 5, NULL);
}

void Sensor::run() {
  for(;;) {
    update();
    info();
    app->publish(SENSOR_UPDATED);
    delay();
  }
}

void Sensor::update() {
  esp_err_t res = read();
  if (res != ESP_OK) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    res = read();
  }
  res == ESP_OK ? success() : failure();
}

esp_err_t Sensor::read() {
  return dht_read_float_data(type, gpio, &humidity, &temperature);
}

void Sensor::success() {
  failed = false;
  failures = 0;
  app->removeError(ERR_SENSOR_READ);
}

void Sensor::failure() {
  failed = true;
  int max_failures = settings->getSensorMaxFailures();
  ESP_LOGE(TAG, "Failed to read sensor %i/%i.", failures, max_failures);
  if (failures >= max_failures) {
    ESP_LOGE(TAG, "Too many sensor failures, rebooting ...");
    esp_restart();
  }
  temperature = -1;
  humidity = -1;
  failures++;
  app->addError(ERR_SENSOR_READ);
}

void Sensor::info() {
  const char* temp = ftoa(temperature);
  const char* humid = ftoa(humidity);
  ESP_LOGI(TAG, "Sensor updated. Temperature: %s, humidity: %s", temp, humid);
  delete temp;
  delete humid;
}

void Sensor::delay() {
  int count = failed ? 3 : 1;
  for (int i = 0; i < count; i++) {
    vTaskDelay(settings->getSensorUpdateInterval() / portTICK_PERIOD_MS);
  }
}
