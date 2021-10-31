#ifndef EVENTS_H
#define EVENTS_H

#include "esp_event.h"
#include "esp_log.h"

ESP_EVENT_DECLARE_BASE(EVENTS);

enum {
  BOOT_PROGRESS,
  SENSOR_UPDATED,
  HEATING_UPDATED,
  SETTINGS_UPDATED
};

class Events {
  public:

    void init();
    void listen(int32_t event, esp_event_handler_t handler); // , void *event_handler_arg
    void post(int32_t event);

  private:

    esp_event_loop_handle_t loop;
};
#endif
