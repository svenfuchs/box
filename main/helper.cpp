#include "helper.h"
#include "freertos/task.h"

#define SYSTEM_INFO_DELAY 15000

// static const char *TAG = "helper";

const tm* now() {
  time_t now;
  time(&now);
  return gmtime(&now);
}

const char* ftoa(float num) {
  return ftoa(num, 1);
}

const char* ftoa(float num, int decs) {
  int factor = power(10, decs);
  int integer = (int) num;
  int decimals = (int) (num * factor) % factor;
  char* buf = (char*) malloc(16);
  sprintf(buf, "%i.%i", integer, decimals);
  return buf;
}

int power(int base, int exp){
  int result = 1;
  while(exp) { result *= base; exp--; }
  return result;
}

void logSystemInfo() {
  xTaskCreate([](void*) {
    for(;;) {
      uint32_t heap = esp_get_free_heap_size();
      uint32_t min_heap = esp_get_minimum_free_heap_size();
      ESP_LOGI(" [mem]", "Free heap: %d (min: %d)", heap, min_heap);

      TaskStatus_t *tasks;
      UBaseType_t count = uxTaskGetNumberOfTasks();
      tasks = (TaskStatus_t*) pvPortMalloc(count * sizeof(TaskStatus_t));
      count = uxTaskGetSystemState(tasks, count, nullptr);
      for(UBaseType_t i = 0; i < count; i++) {
        if(strcmp(tasks[i].pcTaskName, "IDLE") == 0) continue;
        if(tasks[i].pcTaskName == NULL) continue;
        ESP_LOGI(" [mem]", "%-12s %i", tasks[i].pcTaskName, tasks[i].usStackHighWaterMark);
      }
      vPortFree(tasks);

      vTaskDelay(SYSTEM_INFO_DELAY / portTICK_PERIOD_MS);
    }
  }, "info", 1024 * 8, NULL, 5, NULL);
}
