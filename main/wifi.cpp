#include "wifi.h"

// IPAddress LOCAL_IP(192, 168, 178, 72);
// IPAddress GATEWAY(192, 168, 178, 1);
// IPAddress SUBNET(255, 255, 255, 0);
// IPAddress PRIMARY_DNS(8, 8, 8, 8);

#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR 2
#define MAX_IP6_ADDRS_PER_NETIF (5)

static xSemaphoreHandle ips;

static esp_ip4_addr_t ip;
static esp_ip6_addr_t ipv6;
static const char *ipv6Types[] = {
  "ESP_IP6_ADDR_IS_UNKNOWN",
  "ESP_IP6_ADDR_IS_GLOBAL",
  "ESP_IP6_ADDR_IS_LINK_LOCAL",
  "ESP_IP6_ADDR_IS_SITE_LOCAL",
  "ESP_IP6_ADDR_IS_UNIQUE_LOCAL",
  "ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6"
};

static const char *TAG = "[wifi]";

Wifi::Wifi(App* a, Settings::Store* s) {
  app = a;
  settings = s;
}

void Wifi::connect() {
  ESP_LOGI(TAG, "Starting WiFi connection to %s...", settings->getWifiSsid());
  init();
  createNetif();
  registerHandlers();
  configure();
  start();
  waitForIps();
  logInfo();
}

void Wifi::init() {
  esp_netif_init();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
}

void Wifi::createNetif() {
  char *desc;
  esp_netif_inherent_config_t cfg = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
  asprintf(&desc, "%s: %s", TAG, cfg.if_desc);
  cfg.if_desc = desc;
  cfg.route_prio = 128;
  netif = esp_netif_create_wifi(WIFI_IF_STA, &cfg);
  free(desc);
}

void Wifi::configure() {
  wifi_config_t cfg;
  memset(&cfg, 0, sizeof(cfg));
  strncpy((char*) cfg.sta.ssid, settings->getWifiSsid(), 32);
  strncpy((char*) cfg.sta.password, settings->getWifiPassword(), 64);

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_config(WIFI_IF_STA, &cfg);
  esp_wifi_set_ps(WIFI_PS_NONE);
}

void Wifi::registerHandlers() {
  esp_wifi_set_default_wifi_sta_handlers();

  esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, &onStart, netif);
  esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &onConnect, netif);
  esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &Wifi::onDisconnect, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::onIp, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &onIpv6, NULL);
}

void Wifi::start() {
  esp_wifi_start();
}

void Wifi::onStart(void *arg, esp_event_base_t base, int32_t id, void *data) {
  esp_wifi_connect();
}

void Wifi::onConnect(void *esp_netif, esp_event_base_t base, int32_t id, void *data) {
  esp_netif_create_ip6_linklocal((esp_netif_t*) esp_netif);
}

void Wifi::onDisconnect(void *arg, esp_event_base_t base, int32_t id, void *data) {
  ESP_LOGW(TAG, "Wi-Fi disconnected, trying to reconnect...");
  esp_err_t err = esp_wifi_connect();
  if (err != ESP_ERR_WIFI_NOT_STARTED) ESP_ERROR_CHECK(err);
}

void Wifi::onIp(void *arg, esp_event_base_t base, int32_t id, void *data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t*) data;
  if (!isOwnNetif(TAG, event->esp_netif)) {
    ESP_LOGW(TAG, "Got IPv4 from another interface \"%s\": ignored", esp_netif_get_desc(event->esp_netif));
    return;
  }
  ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
  memcpy(&ip, &event->ip_info.ip, sizeof(ip));
  xSemaphoreGive(ips);
}

void Wifi::onIpv6(void *arg, esp_event_base_t base, int32_t id, void *data) {
  ip_event_got_ip6_t* event = (ip_event_got_ip6_t*) data;
  if (!isOwnNetif(TAG, event->esp_netif)) {
    ESP_LOGW(TAG, "Got IPv6 from another netif: ignored");
    return;
  }
  esp_ip6_addr_type_t type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
  ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif), IPV62STR(event->ip6_info.ip), ipv6Types[type]);
  if (type == ESP_IP6_ADDR_IS_LINK_LOCAL) {
    memcpy(&ipv6, &event->ip6_info.ip, sizeof(ipv6));
    xSemaphoreGive(ips);
  }
}

void Wifi::logInfo() {
  esp_netif_t *netif = NULL;
  esp_netif_ip_info_t ip;
  for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
    netif = esp_netif_next(netif);
    if (isOwnNetif(TAG, netif)) {
      ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
      ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));

      ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip.ip));
      esp_ip6_addr_t ip6[MAX_IP6_ADDRS_PER_NETIF];
      int addrs = esp_netif_get_all_ip6(netif, ip6);
      for (int j = 0; j < addrs; ++j) {
        esp_ip6_addr_type_t type = esp_netif_ip6_get_addr_type(&(ip6[j]));
        ESP_LOGI(TAG, "- IPv6 address: " IPV6STR ", type: %s", IPV62STR(ip6[j]), ipv6Types[type]);
      }
    }
  }
}

void Wifi::waitForIps() {
  ips = xSemaphoreCreateCounting(NR_OF_IP_ADDRESSES_TO_WAIT_FOR, 0);
  ESP_LOGI(TAG, "Waiting for IP(s)");
  for (int i = 1; i <= NR_OF_IP_ADDRESSES_TO_WAIT_FOR; ++i) {
    xSemaphoreTake(ips, portMAX_DELAY);
    ESP_LOGI(TAG, "Number of IPs received: %i", i);
  }
}


bool Wifi::isOwnNetif(const char *prefix, esp_netif_t *netif) {
  return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}

esp_netif_t* Wifi::netifFromDesc(const char *desc) {
  esp_netif_t *netif = NULL;
  char *expected_desc;
  asprintf(&expected_desc, "%s: %s", TAG, desc);
  while ((netif = esp_netif_next(netif)) != NULL) {
    if (strcmp(esp_netif_get_desc(netif), expected_desc) == 0) {
      free(expected_desc);
      return netif;
    }
  }
  free(expected_desc);
  return netif;
}
