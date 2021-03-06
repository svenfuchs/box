#ifndef UPD_LOG_H
#define UPD_LOG_H
#include <string.h>
#include "errno.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "app.h"
#include "helper.h"
#include "settings.h"

extern App app;
class Logger;
extern Logger logger;

class Logger {
  public:

    Logger(App*, Settings::Store*);
    void init();
    void run();
    void push(char*);
    void stop();

  private:

    Settings::Store* settings;
    QueueHandle_t queue;
    sockaddr_in dest;
    int sock = 0;

    char* pop();
    void send(char*);
    static int vprintf(const char*, va_list);

    char* format(char*);
    int severity(char);
    int pri(char);
};

#endif
