#include "metrics.h"

#define HTTP_MAX_CONTENT_LENGTH 4096

const char* TAG = "[metr]";

Metrics::Metrics(App* a, Settings::Store* s, Heating* h) {
  app = a;
  heating = h;
  settings = s;
}

void Metrics::init() {
  xTaskCreate([](void*) { metrics.run(); }, "metrics", 1024 * 16, NULL, 5, NULL);
}

void Metrics::run() {
  for(;;) {
    if(validTemp()) {
      ESP_LOGI(TAG, "Sending metrics.");
      const char *data = capture();
      Response response = post(settings->getMetricsUrl(), data);
      if(response.status == 201) {
        handle(response.body);
      }
      delete data;
    }
    delay();
  }
}

bool Metrics::validTemp() {
  return heating->currentTemp > -1;
}

const char* Metrics::capture() {
  char time[64];
  strftime(time, sizeof(time), "%Y-%m-%dT%H:%M:%S%z", now());
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "time", time);
  cJSON_AddNumberToObject(json, "current_temp", heating->currentTemp);
  cJSON_AddNumberToObject(json, "target_temp", heating->targetTemp);
  cJSON_AddBoolToObject(json, "heating", heating->active);
  cJSON_AddStringToObject(json, "token", settings->getMetricsToken());
  char *str = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);
  return str;
}

Response Metrics::post(const char* url, const char* data) {
  char body[HTTP_MAX_CONTENT_LENGTH] = {0};

  esp_http_client_config_t cfg = {};
  cfg.method = HTTP_METHOD_POST;
  cfg.url = url;
  cfg.crt_bundle_attach = esp_crt_bundle_attach;

  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  esp_http_client_set_header(client, "Content-Type", "application/json");

  esp_err_t err = esp_http_client_open(client, strlen(data));;
  if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
  }
  esp_http_client_write(client, data, strlen(data));
  esp_http_client_fetch_headers(client);
  int status = esp_http_client_get_status_code(client);
  esp_http_client_read_response(client, body, HTTP_MAX_CONTENT_LENGTH);
  esp_http_client_cleanup(client);

  Response response = { status, body };
  return response;
}

void Metrics::handle(const char* body) {
  if(!settings->set(settings->parse(body))) return;
  ESP_LOGI(TAG, "SETTINGS UPDATED");
  app->publish(SETTINGS_UPDATED);
}

void Metrics::delay() {
  vTaskDelay(settings->getMetricsNotifyInterval() / portTICK_PERIOD_MS);
}
