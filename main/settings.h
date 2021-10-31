#ifndef STORE_H
#define STORE_H

namespace Settings {
  class Store;
}

#include <string.h>
#include <memory>
#include <type_traits>
#include "cJSON.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "app.h"

namespace Settings {
  struct Data {
    float heatingTargetTemp;
    int heatingMaxDuration;
    int heatingMinPause;
    int heatingUpdateInterval;
    const char* metricsUrl;
    const char* metricsToken;
    int metricsNotifyInterval;
    const char* otaUrl;
    int otaUpdateInterval;
    int sensorMaxFailures;
    int sensorUpdateInterval;
    const char* udpLogAddress;
    int udpLogPort;
    const char* wifiSsid;
    const char* wifiPassword;
  };

  class Store {
    public:

      App* app;

      Store(App*);
      void init();
      bool set(Data);
      Data parse(const char* str);

      float getHeatingTargetTemp();
      int getHeatingMaxDuration();
      int getHeatingMinPause();
      int getHeatingUpdateInterval();
      const char* getMetricsUrl();
      const char* getMetricsToken();
      int getMetricsNotifyInterval();
      const char* getOtaUrl();
      int getOtaUpdateInterval();
      int getSensorMaxFailures();
      int getSensorUpdateInterval();
      const char* getUdpLogAddress();
      int getUdpLogPort();
      const char* getWifiSsid();
      const char* getWifiPassword();

      bool setHeatingTargetTemp(float value);
      bool setHeatingMaxDuration(int value);
      bool setHeatingMinPause(int value);
      bool setHeatingUpdateInterval(int value);
      bool setMetricsUrl(const char* value);
      bool setMetricsToken(const char* value);
      bool setMetricsNotifyInterval(int value);
      bool setOtaUrl(const char* value);
      bool setOtaUpdateInterval(int value);
      bool setSensorMaxFailures(int value);
      bool setSensorUpdateInterval(int value);
      bool setUdpLogAddress(const char* value);
      bool setUdpLogPort(int value);
      bool setWifiSsid(const char* value);
      bool setWifiPassword(const char* value);

    private:

      nvs_handle_t handle;
      esp_err_t res;
      Data data;

      void open();
      void read();
      void commit();

      int getInt(const char* key, const int default_);
      float getFloat(const char* key, const float default_);
      const char* getString(const char* key, const char* default_);

      void putInt(const char* key, const int value);
      void putFloat(const char* key, const float value);
      void putString(const char* key, const char* value);

      size_t getSize(const char* key);
  };
}
#endif
