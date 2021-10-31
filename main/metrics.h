#ifndef METRICS_H
#define METRICS_H

class Metrics;
extern Metrics metrics;

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "app.h"
#include "heating.h"
#include "helper.h"
#include "settings.h"

struct Metric {
  const tm* time;
  float temperature;
  float humidity;
};

struct Response {
  int status;
  const char* body;
};

class Metrics {
  public:
    Metrics(App*, Settings::Store*, Heating*);

    void init();
    void run();

  private:

    App* app;
    Heating* heating;
    Settings::Store* settings;

    const char* capture();
    bool validTemp();
    Response post(const char* url, const char*);
    void handle(const char* body);
    void delay();
};
#endif
