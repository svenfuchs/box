#include "logger.h"

#define LOG_MSG_MAX_LEN 2048
#define LOG_QUEUE_SIZE 2048

static const char *TAG = "[logr]";

Logger::Logger(App* a, Settings::Store* s) {
  // how to not use a global var app?
  // app = a;
  settings = s;
  queue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(char*));
}

void Logger::init() {
  ESP_LOGI(TAG, "Initializing logging ...");

  const char* address = settings->getUdpLogAddress();
  int port = settings->getUdpLogPort();
  struct hostent *host = gethostbyname(address);

  dest.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *) (host->h_addr_list[0])));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(port);

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return;
  }

  esp_log_set_vprintf(Logger::vprintf);
  xTaskCreate([](void*) { logger.run(); }, "logger", 1024 * 8, NULL, 5, NULL);
}

void Logger::run() {
  while (true) {
    char* msg = pop();
    send(msg);
    free((void*) msg);
  }
}

void Logger::stop() {
  esp_log_set_vprintf(::vprintf);
  xQueueReset(queue);
	shutdown(sock, 0);
	close(sock);
}

void Logger::push(char* msg) {
  BaseType_t res = xQueueSendToBack(queue, (void*) &msg, (TickType_t) 0 / portTICK_PERIOD_MS);
  if(res == errQUEUE_FULL) printf("Error: Log message queue full\n");
}

char* Logger::pop() {
	char* msg;
	BaseType_t res = xQueueReceive(queue, &msg, (TickType_t) portMAX_DELAY);
  if(res != pdPASS) msg = NULL;
  return msg;
}

void Logger::send(char* msg) {
  if (msg == NULL) return;
  int res = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &dest, sizeof(dest));
  if (res >= 0) {
    app.removeError(ERR_UDP_LOG);
  } else {
    app.addError(ERR_UDP_LOG);
    printf("Failed to send UDP message: %s (%i)\n", strerror(errno), errno);
  }
}

int Logger::vprintf(const char *fmt, va_list args) {
	char *buf = (char*) malloc(sizeof(char) * LOG_MSG_MAX_LEN);
	vsprintf(buf, fmt, args);
	logger.push(buf);
  return ::vprintf(fmt, args);
}
