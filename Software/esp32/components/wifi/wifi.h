#ifndef WIFI_H
#define WIFI_H
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "adc.h"
#include "heater.h"
#include "beep.h"
#define ESP_WIFI_SSID      "原神启动区"
#define ESP_WIFI_PASS      "woaiwanyuan"
#define ESP_MAXIMUM_RETRY   5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER ""

#define THINGSBOARD_SERVER "192.168.31.71:8080"
#define ACCESS_TOKEN "r8ya20c3vpww62ukwea0"
#define RPC_TIMEOUT_MS 10000


void wifi_init_sta(void);
void send_temp();
void set_timezone(void);
void initialize_sntp(void);
void wait_for_time_sync(void);
void http_long_poll_task(void *pvParameters);


#endif