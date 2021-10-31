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
  ESP_LOGI(TAG, "Init logging ...");
  app.progress(INIT_LOGGER);

  const char* address = settings->getUdpLogAddress();
  int port = settings->getUdpLogPort();
  struct hostent *host = gethostbyname(address);
  if(host == NULL){
    ESP_LOGE(TAG, "Unable to resolve hostname");
  }

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
  // msg = format(msg);
  int res = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &dest, sizeof(dest));
  if (res >= 0) {
    app.removeError(ERR_UDP_LOG);
  } else {
    app.addError(ERR_UDP_LOG);
    printf("Failed to send UDP message: %s (%i)\n", strerror(errno), errno);
  }
}

int Logger::vprintf(const char* fmt, va_list args) {
  char* buf = (char*) malloc(sizeof(char) * LOG_MSG_MAX_LEN);
  vsprintf(buf, fmt, args);
  logger.push(buf);
  return ::vprintf(fmt, args);
}

int Logger::severity(char level) {
  switch(level) {
    case 'E':
      return 3;
    case 'W':
      return 4;
    case 'I':
      return 5;
    case 'D':
      return 6;
    case 'V':
      return 7;
    default:
      printf("Unknown severity level: <%c>\n", level);
      return 3;
  }
}

int Logger::pri(char level) {
  return 128 + severity(level);
}

char* Logger::format(char* buf) {
  char level;
	char color[8] = "";
  if(buf[0] == 27) {
    level = buf[7];
    strncat(color, buf, 7);
  } else {
    level = buf[0];
  }

  char* ptr = strchr(buf, ')');
  if(ptr == nullptr) return buf;
  char* rest = ptr + 2;

  ptr = strchr(rest, ':');
  if(ptr == nullptr) return buf;
  char* msg = ptr + 2;

  char tag[20] = "";
  strncpy(tag, rest, msg - rest - 2);

  time_t now;
  char tm[64];
  time(&now);
  strftime(tm, sizeof(tm), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

	char *line = (char*) malloc(sizeof(char) * 1024);
  sprintf(line, "<%i>1 %s kiste %s - - - %s%c %s\n", pri(level), tm, tag, color, level, msg);
  return line;
}

