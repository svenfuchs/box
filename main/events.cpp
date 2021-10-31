#include "freertos/FreeRTOS.h"
#include "events.h"

ESP_EVENT_DEFINE_BASE(EVENTS);
// static const char *TAG = "[evnt]";

Events::Events(App* a) {
  app = a;
}

void Events::init() {
  esp_event_loop_args_t args = {};
  args.queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE;
  args.task_name = "events";
  args.task_priority = 1;
  args.task_stack_size = 1024 * 16;
  args.task_core_id = 1;
  esp_event_loop_create(&args, &loop);
}

void Events::post(int32_t event) {
  esp_event_post_to(loop, EVENTS, event, NULL, 0, 0);
}

void Events::listen(int32_t event, esp_event_handler_t handler) {
  esp_event_handler_register_with(loop, EVENTS, event, handler, NULL);
}
