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


#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_client.h"
#include "cJSON.h"
#define ESP_WIFI_SSID      "TP-LINK_1002"
#define ESP_WIFI_PASS      "13461002088"
#define ESP_MAXIMUM_RETRY   5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER ""

#define THINGSBOARD_SERVER "http://localhost:8080"
#define ACCESS_TOKEN "TmvoAXAUE6G6XMaFfoGK"



void wifi_init_sta(void);
void send_temp();



#endif