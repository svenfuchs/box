#ifndef HELPER_H
#define HELPER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "esp_log.h"

const tm* now();
const char* ftoa(float num);
const char* ftoa(float num, int decimals);
int power(int base, int exp);
void systemInfo(int delay);
#endif
