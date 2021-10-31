#include "settings.h"

static const char *TAG = "[stor]";

#define HEATING_TARGET_TEMP_KEY "heat.target_temp"
#define HEATING_MAX_DURATION_KEY "heat.max_dur"
#define HEATING_MIN_PAUSE_KEY "heat.min_pause"
#define HEATING_UPDATE_INTERVAL_KEY "heat.interval"
#define METRICS_URL_KEY "metrics.url"
#define METRICS_TOKEN_KEY "metrics.token"
#define METRICS_NOTIFY_INTERVAL_KEY "metrics.interval"
#define OTA_URL_KEY "ota.url"
#define OTA_UPDATE_INTERVAL_KEY "ota.interval"
#define SENSOR_MAX_FAILURES_KEY "sensor.max_fail"
#define SENSOR_UPDATE_INTERVAL_KEY "sensor.interval"
#define UDP_LOG_ADDRESS_KEY "udp_log.address"
#define UDP_LOG_PORT_KEY "udp_log.port"
#define WIFI_SSID_KEY "wifi.ssid"
#define WIFI_PASSWORD_KEY "wifi.password"

using namespace Settings;

void Store::init() {
  open();
  read();
}

float Store::getHeatingTargetTemp() {
  return data.heatingTargetTemp;
}

int Store::getHeatingMaxDuration() {
  return data.heatingMaxDuration;
}

int Store::getHeatingMinPause() {
  return data.heatingMinPause;
}

int Store::getHeatingUpdateInterval() {
  return data.heatingUpdateInterval;
}

const char* Store::getMetricsUrl() {
  return data.metricsUrl;
}

const char* Store::getMetricsToken() {
  return data.metricsToken;
}

int Store::getMetricsNotifyInterval() {
  return data.metricsNotifyInterval;
}

const char* Store::getOtaUrl() {
  return data.otaUrl;
}

int Store::getOtaUpdateInterval() {
  return data.otaUpdateInterval;
}

int Store::getSensorMaxFailures() {
  return data.sensorMaxFailures;
}

int Store::getSensorUpdateInterval() {
  return data.sensorUpdateInterval;
}

const char* Store::getUdpLogAddress() {
  return data.udpLogAddress;
}

int Store::getUdpLogPort() {
  return data.udpLogPort;
}

const char* Store::getWifiSsid() {
  return data.wifiSsid;
}

const char* Store::getWifiPassword() {
  return data.wifiPassword;
}

void Store::read() {
  data.heatingTargetTemp = getFloat(HEATING_TARGET_TEMP_KEY, CONFIG_HEATING_TARGET_TEMP);
  data.heatingMaxDuration = getInt(HEATING_MAX_DURATION_KEY, CONFIG_HEATING_MAX_DURATION);
  data.heatingMinPause = getInt(HEATING_MIN_PAUSE_KEY, CONFIG_HEATING_MIN_PAUSE);
  data.heatingUpdateInterval = getInt(HEATING_UPDATE_INTERVAL_KEY, CONFIG_HEATING_UPDATE_INTERVAL);
  data.metricsUrl = getString(METRICS_URL_KEY, CONFIG_METRICS_URL);
  data.metricsToken = getString(METRICS_TOKEN_KEY, CONFIG_METRICS_TOKEN);
  data.metricsNotifyInterval = getInt(METRICS_NOTIFY_INTERVAL_KEY, CONFIG_METRICS_NOTIFY_INTERVAL);
  data.otaUrl = getString(OTA_URL_KEY, CONFIG_OTA_URL);
  data.otaUpdateInterval = getInt(OTA_UPDATE_INTERVAL_KEY, CONFIG_OTA_UPDATE_INTERVAL);
  data.sensorMaxFailures = getInt(SENSOR_MAX_FAILURES_KEY, CONFIG_SENSOR_MAX_FAILURES);
  data.sensorUpdateInterval = getInt(SENSOR_UPDATE_INTERVAL_KEY, CONFIG_SENSOR_UPDATE_INTERVAL);
  data.udpLogAddress = getString(UDP_LOG_ADDRESS_KEY, CONFIG_UDP_LOG_ADDRESS);
  data.udpLogPort = getInt(UDP_LOG_PORT_KEY, CONFIG_UDP_LOG_PORT);
  data.wifiSsid = getString(WIFI_SSID_KEY, CONFIG_WIFI_SSID);
  data.wifiPassword = getString(WIFI_PASSWORD_KEY, CONFIG_WIFI_PASSWORD);
}

bool Store::set(Data data) {
  bool updated = false;
  updated = setHeatingTargetTemp(data.heatingTargetTemp) || updated;
  updated = setHeatingMaxDuration(data.heatingMaxDuration) || updated;
  updated = setHeatingMinPause(data.heatingMinPause) || updated;
  updated = setHeatingUpdateInterval(data.heatingUpdateInterval) || updated;
  updated = setMetricsUrl(data.metricsUrl) || updated;
  updated = setMetricsToken(data.metricsToken) || updated;
  updated = setMetricsNotifyInterval(data.metricsNotifyInterval) || updated;
  updated = setOtaUrl(data.otaUrl) || updated;
  updated = setOtaUpdateInterval(data.otaUpdateInterval) || updated;
  updated = setSensorMaxFailures(data.sensorMaxFailures) || updated;
  updated = setSensorUpdateInterval(data.sensorUpdateInterval) || updated;
  updated = setUdpLogAddress(data.udpLogAddress) || updated;
  updated = setUdpLogPort(data.udpLogPort) || updated;
  updated = setWifiSsid(data.wifiSsid) || updated;
  updated = setWifiPassword(data.wifiPassword) || updated;
  return updated;
}

Data Store::parse(const char* str) {
  cJSON *json = cJSON_Parse(str);
  Data data = {};

  cJSON* node = cJSON_GetObjectItemCaseSensitive(json, "target_temp");
  if(cJSON_IsNumber(node)) data.heatingTargetTemp = node->valuedouble;

  node = cJSON_GetObjectItemCaseSensitive(json, "heating_duration");
  if(cJSON_IsNumber(node)) data.heatingMaxDuration = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "heating_pause");
  if(cJSON_IsNumber(node)) data.heatingMinPause = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "heating_update_interval");
  if(cJSON_IsNumber(node)) data.heatingUpdateInterval = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "metrics_url");
  if(cJSON_IsString(node)) data.metricsUrl = node->valuestring;

  node = cJSON_GetObjectItemCaseSensitive(json, "metrics_token");
  if(cJSON_IsString(node)) data.metricsToken = node->valuestring;

  node = cJSON_GetObjectItemCaseSensitive(json, "metrics_notify_interval");
  if(cJSON_IsNumber(node)) data.metricsNotifyInterval = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "ota_url");
  if(cJSON_IsString(node)) data.otaUrl = node->valuestring;

  node = cJSON_GetObjectItemCaseSensitive(json, "ota_update_interval");
  if(cJSON_IsNumber(node)) data.otaUpdateInterval = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "sensor_max_failures");
  if(cJSON_IsNumber(node)) data.sensorMaxFailures = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "sensor_update_interval");
  if(cJSON_IsNumber(node)) data.sensorUpdateInterval = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "udp_log_address");
  if(cJSON_IsString(node)) data.udpLogAddress = node->valuestring;

  node = cJSON_GetObjectItemCaseSensitive(json, "udp_log_port");
  if(cJSON_IsNumber(node)) data.udpLogPort = node->valueint;

  node = cJSON_GetObjectItemCaseSensitive(json, "wifi_ssid");
  if(cJSON_IsString(node)) data.wifiSsid = node->valuestring;

  node = cJSON_GetObjectItemCaseSensitive(json, "wifi_password");
  if(cJSON_IsString(node)) data.wifiPassword = node->valuestring;

  cJSON_Delete(json);
  return data;
}

bool Store::setHeatingTargetTemp(float value) {
  if(!value || getHeatingTargetTemp() == value) return false;
  ESP_LOGI(TAG, "Setting heating target temp to %f", value);
  data.heatingTargetTemp = value;
  putFloat(HEATING_TARGET_TEMP_KEY, value);
  return true;
}

bool Store::setHeatingMaxDuration(int value) {
  if(!value || getHeatingMaxDuration() == value) return false;
  ESP_LOGI(TAG, "Setting heating max duration to %i", value);
  data.heatingMaxDuration = value;
  putInt(HEATING_MAX_DURATION_KEY, value);
  return true;
}

bool Store::setHeatingMinPause(int value) {
  if(!value || getHeatingMinPause() == value) return false;
  ESP_LOGI(TAG, "Setting heating min pause to %i", value);
  data.heatingMinPause = value;
  putInt(HEATING_MIN_PAUSE_KEY, value);
  return true;
}

bool Store::setHeatingUpdateInterval(int value) {
  if(!value || getHeatingUpdateInterval() == value) return false;
  ESP_LOGI(TAG, "Setting heating update interval to %i", value);
  data.heatingUpdateInterval = value;
  putInt(HEATING_UPDATE_INTERVAL_KEY, value);
  return true;
}

bool Store::setMetricsUrl(const char* value) {
  if(!value || getMetricsUrl() == value) return false;
  ESP_LOGI(TAG, "Setting metrics url to %s", value);
  data.metricsUrl = value;
  putString(METRICS_URL_KEY, value);
  return true;
}

bool Store::setMetricsToken(const char* value) {
  if(!value || getMetricsToken() == value) return false;
  ESP_LOGI(TAG, "Setting metrics token to %s", value);
  data.metricsToken = value;
  putString(METRICS_TOKEN_KEY, value);
  return true;
}

bool Store::setMetricsNotifyInterval(int value) {
  if(!value || getMetricsNotifyInterval() == value) return false;
  ESP_LOGI(TAG, "Setting metrics notify interval to %i", value);
  data.metricsNotifyInterval = value;
  putInt(METRICS_NOTIFY_INTERVAL_KEY, value);
  return true;
}

bool Store::setOtaUrl(const char* value) {
  if(!value || getOtaUrl() == value) return false;
  ESP_LOGI(TAG, "Setting ota url to %s", value);
  data.otaUrl = value;
  putString(OTA_URL_KEY, value);
  return true;
}

bool Store::setOtaUpdateInterval(int value) {
  if(!value || getOtaUpdateInterval() == value) return false;
  ESP_LOGI(TAG, "Setting ota interval to %i", value);
  data.otaUpdateInterval = value;
  putInt(OTA_UPDATE_INTERVAL_KEY, value);
  return true;
}

bool Store::setSensorMaxFailures(int value) {
  if(!value || getSensorMaxFailures() == value) return false;
  ESP_LOGI(TAG, "Setting sensor max failures to %i", value);
  data.sensorMaxFailures = value;
  putInt(SENSOR_MAX_FAILURES_KEY, value);
  return true;
}

bool Store::setSensorUpdateInterval(int value) {
  if(!value || getSensorUpdateInterval() == value) return false;
  ESP_LOGI(TAG, "Setting sensor update interval to %i", value);
  data.sensorUpdateInterval = value;
  putInt(SENSOR_UPDATE_INTERVAL_KEY, value);
  return true;
}

bool Store::setUdpLogAddress(const char* value) {
  if(!value || getUdpLogAddress() == value) return false;
  ESP_LOGI(TAG, "Setting udp log address to %s", value);
  data.udpLogAddress = value;
  putString(UDP_LOG_ADDRESS_KEY, value);
  return true;
}

bool Store::setUdpLogPort(int value) {
  if(!value || getUdpLogPort() == value) return false;
  ESP_LOGI(TAG, "Setting udp log port to %i", value);
  data.udpLogPort = value;
  putInt(UDP_LOG_PORT_KEY, value);
  return true;
}

bool Store::setWifiSsid(const char* value) {
  if(!value || getWifiSsid() == value) return false;
  ESP_LOGI(TAG, "Setting wifi ssid to %s", value);
  data.wifiSsid = value;
  putString(WIFI_SSID_KEY, value);
  return true;
}

bool Store::setWifiPassword(const char* value) {
  if(!value || getWifiPassword() == value) return false;
  ESP_LOGI(TAG, "Setting wifi password to %s", value);
  data.wifiPassword = value;
  putString(WIFI_PASSWORD_KEY, value);
  return true;
}

void Store::putInt(const char* key, const int value) {
  res = nvs_set_i32(handle, key, value);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Error putting int to %s: %s", key, esp_err_to_name(res));
  }
  commit();
}

void Store::putFloat(const char* key, const float value) {
  res = nvs_set_blob(handle, key, (void*)&value, sizeof(float));
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Error putting float to %s: %s", key, esp_err_to_name(res));
  }
  commit();
}

void Store::putString(const char* key, const char* value) {
  res = nvs_set_str(handle, key, value);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Error putting string to %s: %s", key, esp_err_to_name(res));
  }
  commit();
}

int Store::getInt(const char* key, const int default_) {
  int value = default_;
  nvs_get_i32(handle, key, &value);
  return value;
}

float Store::getFloat(const char* key, const float default_) {
  size_t size = getSize(key);
  if(!size) return default_;

  float value = default_;
  res = nvs_get_blob(handle, key, &value, &size);
  if (res == ESP_OK) return value;

  ESP_LOGE(TAG, "Error reading float from %s: %s", key, esp_err_to_name(res));
  return default_;
}

const char* Store::getString(const char* key, const char* default_) {
  size_t size;
  nvs_get_str(handle, key, NULL, &size);
  if (res != ESP_OK) return default_;

  char* value = (char*) malloc(size);
  res = nvs_get_str(handle, key, value, &size);
  if (res != ESP_OK) return default_;

  return value;
}

size_t Store::getSize(const char* key){
  size_t size = 0;
  nvs_get_blob(handle, key, NULL, &size);
  return size;
}

void Store::open() {
  res = nvs_open("storage", NVS_READWRITE, &handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs; %s", esp_err_to_name(res));
  }
}

void Store::commit() {
  res = nvs_commit(handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Error committing to nvs: %s.", esp_err_to_name(res));
  }
}
