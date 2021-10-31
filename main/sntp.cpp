#include "sntp.h"

static const char* TAG = "time";
const int RETRIES = 20;

void Sntp::init() {
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_setservername(1, "europe.pool.ntp.org");
  sntp_setservername(2, "uk.pool.ntp.org ");
  sntp_setservername(3, "us.pool.ntp.org");
  sntp_setservername(4, "time1.google.com");
  // sntp_servermode_dhcp(1);
  sntp_init();

  for(int i = 0; !isSynced() && i < RETRIES; i++) {
    ESP_LOGI(TAG, "Syncing time ... (%d/%d)", i, RETRIES);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  if(isSynced()) {
    ESP_LOGI(TAG, "Done syncing time.");
  } else {
    ESP_LOGE(TAG, "Failed to sync time. Restarting ...");
    esp_restart();
  }
}

bool Sntp::isSynced() {
  return time(nullptr) > 1000000000ul;
}

