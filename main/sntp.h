#ifndef SNTP_H
#define SNTP_H

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "app.h"

const tm* now();

class Sntp {
  public:

    Sntp(App*);
    void init();

  private:

    App* app;
    bool isSynced();
};
#endif
