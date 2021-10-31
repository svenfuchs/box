#include "display.h"

#define PIN_SDA GPIO_NUM_21
#define PIN_SCL GPIO_NUM_22

static const char *TAG = "[dspl]";

Display::Display(App* a, Settings::Store* st, Heating* h, Sensor* sn) {
  app = a;
  settings = st;
  heating = h;
  sensor = sn;
}

void Display::init() {
  ESP_LOGI(TAG, "Display init.");
  u8g2_esp32_hal_t cnf;
  memset(&cnf, 0, sizeof(u8g2_esp32_hal_t));
  cnf.sda = PIN_SDA;
  cnf.scl = PIN_SCL;
  u8g2_esp32_hal_init(cnf);

  u8g2_Setup_ssd1306_i2c_128x64_noname_f(
    &u8g2,
    U8G2_R0,
    u8g2_esp32_i2c_byte_cb,
    u8g2_esp32_gpio_and_delay_cb
  );

  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);

  app->listen(this, BOOT_PROGRESS);
  app->listen(this, SENSOR_UPDATED);
  app->listen(this, HEATING_UPDATED);
  app->listen(this, SETTINGS_UPDATED);

  drawBootScreen();
}

void Display::update(int32_t event) {
  if(app->isBooting()) {
    drawBootScreen();
  } else {
    drawTempScreen();
  }
}

void Display::clear() {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SendBuffer(&u8g2);
}

void Display::drawBootScreen() {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_open_iconic_human_2x_t);
  u8g2_DrawGlyph(&u8g2, 60, 28, 66);

  u8g2_SetFont(&u8g2, u8g2_font_helvR08_tf);
  switch(app->state) {
    case BOOTING_INIT:
      u8g2_DrawStr(&u8g2, 43, 52, "Booting ...");
      break;
    case BOOTING_WIFI:
      u8g2_DrawStr(&u8g2, 43, 52, "Init WiFi ...");
      break;
    case BOOTING_SNTP:
      u8g2_DrawStr(&u8g2, 40, 52, "Sync time ...");
      break;
  }
  u8g2_SendBuffer(&u8g2);
}

void Display::drawTempScreen() {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, u8g2_font_helvR10_tf);

  char* str = formatTemp(heating->targetTemp);
  u8g2_DrawStr(&u8g2, 10, 18, str);
  delete str;

  str = formatTemp(heating->currentTemp);
  u8g2_DrawStr(&u8g2, 10, 38, str);
  delete str;

  str = formatHumid(sensor->humidity);
  u8g2_DrawStr(&u8g2, 10, 58, str);
  delete str;

  if(heating->active) {
    u8g2_DrawDisc(&u8g2, 119, 11, 3, U8G2_DRAW_ALL);
  } else {
    // u8g2_DrawCircle(&u8g2, 119, 11, 3, U8G2_DRAW_ALL);
  }

  u8g2_SetFont(&u8g2, u8g2_font_open_iconic_human_1x_t);
  u8g2_DrawGlyph(&u8g2, 115, 58, 66);

  if(app->hasErrors()) {
    u8g2_SetFont(&u8g2, u8g2_font_helvR08_tf);
    str = formatErrors(app->errors);
    u8g2_DrawStr(&u8g2, 106, 42, str);
    delete str;

    u8g2_SetFont(&u8g2, u8g2_font_open_iconic_embedded_1x_t);
    u8g2_DrawGlyph(&u8g2, 116, 42, 71);
  }

  u8g2_SendBuffer(&u8g2);
}

char* Display::formatTemp(float value) {
  char* buf = (char*) malloc(16);
  const char* val = ftoa(value);
  sprintf(buf, "%s C",  val);
  delete val;
  return buf;
}

char* Display::formatHumid(float value) {
  char* buf = (char*) malloc(16);
  const char* val = ftoa(value);
  sprintf(buf, "%s %%",  val);
  delete val;
  return buf;
}

char* Display::formatErrors(int code) {
  char* buf = (char*) malloc(16);
  sprintf(buf, "%i", code);
  return buf;
}
