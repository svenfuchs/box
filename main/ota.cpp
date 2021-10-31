#include "ota.h"

static const char *TAG = " [ota]";
// extern const uint8_t cert_start[] asm("_binary_cert_pem_start");
// extern const uint8_t cert_end[] asm("_binary_cert_pem_end");

Ota::Ota(App* a, Settings::Store* s) {
  app = a;
  settings = s;
}

void Ota::init() {
  xTaskCreate([](void*) { ota.run(); }, "ota", 1024 * 8, NULL, 5, NULL);
}

void Ota::run() {
  for(;;) {
    ESP_LOGI(TAG, "Checking for firmware update ...");

    if (start() && validate()) {
      read();
      finish();
    } else {
      esp_https_ota_abort(handle);
    }

    if (!errored) {
      app->removeError(ERR_HTTP_OTA);
    } else {
      app->addError(ERR_HTTP_OTA);
    }

    vTaskDelay(settings->getOtaUpdateInterval() / portTICK_PERIOD_MS);
  }
}

bool Ota::start() {
  errored = false;

  esp_http_client_config_t http_cfg = {};
  http_cfg.url = settings->getOtaUrl();
  http_cfg.timeout_ms = 5000;
  esp_https_ota_config_t cfg = {};
  cfg.http_config = &http_cfg;
  // http_cfg.cert_pem = (char*) cert_start;
  // cfg.partial_http_download = true;

  esp_err_t err = esp_https_ota_begin(&cfg, &handle);
  if (err == ESP_OK) return true;

  ESP_LOGE(TAG, "Failed to start OTA update (%s)", cfg.http_config->url);
  errored = true;
  return false;
}

bool Ota::validate() {
  esp_app_desc_t desc;
  ESP_LOGI(TAG, "Fetching firmware image description ...");
  esp_err_t err = esp_https_ota_get_img_desc(handle, &desc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to fetch firmware image description.");
    errored = true;
    return false;
  }

  const esp_partition_t *running = esp_ota_get_running_partition();
  esp_app_desc_t current;
  if (esp_ota_get_partition_description(running, &current) == ESP_OK) {
    ESP_LOGI(TAG, "Current firmware version: %s", current.version);
  }

  if (memcmp(desc.version, current.version, sizeof(desc.version)) == 0) {
    ESP_LOGI(TAG, "Firmware version is up to date.");
    return false;
  }

  ESP_LOGW(TAG, "New firmware version found.");
  return true;
}

void Ota::read() {
  esp_err_t err;
  int last_progr = 0;
  while (1) {
    err = esp_https_ota_perform(handle);
    if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) break;
    int total = esp_https_ota_get_image_size(handle);
    int done = esp_https_ota_get_image_len_read(handle);
    int progr = (int) ((float) done / (float) total * 100);
    if (progr <= last_progr) continue;
    ESP_LOGW(TAG, "Progress: %d/%d %i%%", done, total, progr);
    last_progr = progr;
  }
}

void Ota::finish() {
  if (esp_https_ota_is_complete_data_received(handle) != true) {
    ESP_LOGE(TAG, "Incomplete data.");
    return;
  }

  esp_err_t result = esp_https_ota_finish(handle);
  if (result == ESP_OK) {
    ESP_LOGW(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
  }

  if (result == ESP_ERR_OTA_VALIDATE_FAILED) {
    ESP_LOGE(TAG, "Image validation failed, image is corrupted");
  }
  ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", result);
}
