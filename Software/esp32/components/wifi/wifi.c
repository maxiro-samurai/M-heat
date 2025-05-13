#include "wifi.h"

const char* server_cert ="-----BEGIN CERTIFICATE-----\n"
"MIIEVzCCAj+gAwIBAgIRAIOPbGPOsTmMYgZigxXJ/d4wDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\n"
"WhcNMjcwMzEyMjM1OTU5WjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
"RW5jcnlwdDELMAkGA1UEAxMCRTUwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQNCzqK\n"
"a2GOtu/cX1jnxkJFVKtj9mZhSAouWXW0gQI3ULc/FnncmOyhKJdyIBwsz9V8UiBO\n"
"VHhbhBRrwJCuhezAUUE8Wod/Bk3U/mDR+mwt4X2VEIiiCFQPmRpM5uoKrNijgfgw\n"
"gfUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcD\n"
"ATASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSfK1/PPCFPnQS37SssxMZw\n"
"i9LXDTAfBgNVHSMEGDAWgBR5tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcB\n"
"AQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0g\n"
"BAwwCjAIBgZngQwBAgEwJwYDVR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVu\n"
"Y3Iub3JnLzANBgkqhkiG9w0BAQsFAAOCAgEAH3KdNEVCQdqk0LKyuNImTKdRJY1C\n"
"2uw2SJajuhqkyGPY8C+zzsufZ+mgnhnq1A2KVQOSykOEnUbx1cy637rBAihx97r+\n"
"bcwbZM6sTDIaEriR/PLk6LKs9Be0uoVxgOKDcpG9svD33J+G9Lcfv1K9luDmSTgG\n"
"6XNFIN5vfI5gs/lMPyojEMdIzK9blcl2/1vKxO8WGCcjvsQ1nJ/Pwt8LQZBfOFyV\n"
"XP8ubAp/au3dc4EKWG9MO5zcx1qT9+NXRGdVWxGvmBFRAajciMfXME1ZuGmk3/GO\n"
"koAM7ZkjZmleyokP1LGzmfJcUd9s7eeu1/9/eg5XlXd/55GtYjAM+C4DG5i7eaNq\n"
"cm2F+yxYIPt6cbbtYVNJCGfHWqHEQ4FYStUyFnv8sjyqU8ypgZaNJ9aVcWSICLOI\n"
"E1/Qv/7oKsnZCWJ926wU6RqG1OYPGOi1zuABhLw61cuPVDT28nQS/e6z95cJXq0e\n"
"K1BcaJ6fJZsmbjRgD5p3mvEf5vdQM7MCEvU0tHbsx2I5mHHJoABHb8KVBgWp/lcX\n"
"GWiWaeOyB7RP+OfDtvi2OsapxXiV7vNVs7fMlrRjY1joKaqmmycnBvAq14AEbtyL\n"
"sVfOS66B8apkeFX2NY4XPEYV4ZSCe8VHPrdrERk2wILG3T/EGmSIkCYVUMSnjmJd\n"
"VQD9F6Na/+zmXCc=\n"
"-----END CERTIFICATE-----";

extern bool wifi_enable;
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */


static const char *TAG = "wifi station";

static int s_retry_num = 0;


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}
// 发送 HTTP POST 请求（用于响应或主动发送 RPC）
static esp_err_t http_post(const char *url, const char *data) {
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, data, strlen(data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "POST Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "POST failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return err;
}

// 处理 RPC 命令（例如控制 GPIO）
static void handle_rpc(const cJSON *rpc) {
     // 发送响应
    char response[64];
    snprintf(response, sizeof(response), "{\"result\":\"OK\"}");
    char url[128];
    cJSON *method = cJSON_GetObjectItem(rpc, "method");
    cJSON *params = cJSON_GetObjectItem(rpc, "params");
    cJSON *id = cJSON_GetObjectItem(rpc, "id");

    if (!method || !params || !id) return;
    
    if (strcmp(method->valuestring, "setHeater") == 0) {
        if (params && cJSON_IsBool(params))
        {
            PWM_state = params->valueint;
            en_pid = params->valueint;
            ESP_LOGI(TAG,"Active: %s", params->valueint ? "true" : "false");
        }
        snprintf(url, sizeof(url), "http://%s/api/v1/%s/rpc/%d", THINGSBOARD_SERVER, ACCESS_TOKEN, id->valueint);
        http_post(url, response);
    }
    else if (strcmp(method->valuestring, "setVolume") == 0){
        if (params && cJSON_IsBool(params))
        {
            Volume = params->valueint;
            ESP_LOGI(TAG,"Active: %s", params->valueint ? "true" : "false");
        }
        snprintf(url, sizeof(url), "http://%s/api/v1/%s/rpc/%d", THINGSBOARD_SERVER, ACCESS_TOKEN, id->valueint);
        http_post(url, response);
    }
    
       
}
static void handle_tb_attributes(const cJSON *rpc){

    cJSON *shared = cJSON_GetObjectItem(rpc, "shared");

    cJSON *Kp = cJSON_GetObjectItem(shared, "Kp");
    cJSON *Ki = cJSON_GetObjectItem(shared, "Ki");
    cJSON *Kd = cJSON_GetObjectItem(shared, "Kd");

    cJSON *temperature = cJSON_GetObjectItem(shared, "temperature");

    if (!shared) return;

    ADC.set_temp = temperature->valueint;
    pid.Kp = Kp->valuedouble;
    pid.Ki = Ki->valuedouble;
    pid.Kd = Kd->valuedouble;

}
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            
            cJSON *data = cJSON_Parse(evt->data);
            if (data) {
                handle_rpc(data);
                handle_tb_attributes(data);
                cJSON_Delete(data);
            }
            ESP_LOGI("HTTP", "Received data: %.*s", evt->data_len, (char*)evt->data);
            break;
        // case HTTP_EVENT_ERROR:
        //     if (evt->data.error_handle->error_type == HTTP_ERROR_TYPE_CONNECT) {
        //         ESP_LOGE(TAG, "Connect Error: %s", 
        //             esp_err_to_name(evt->data.error_handle->esp_tls_last_esp_err));
        //     }
        default:
            break;
    }
    return ESP_OK;
}
void set_timezone(void) {
    // 设置时区（例如：东八区北京时间）
    setenv("TZ", "CST-8", 1);
    tzset();
}

void initialize_sntp(void) {
    // 配置 SNTP 服务器和操作模式
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(
        3,                            // 最大重试次数
        ESP_SNTP_SERVER_LIST("ntp.aliyun.com","pool.ntp.org", "time.google.com") // 多个 NTP 服务器
    );
    config.start = true;              // 立即启动时间同步
    config.server_from_dhcp = false;   // 不通过 DHCP 获取 NTP 服务器
    config.renew_servers_after_new_IP = true; // IP 变更后重新同步

    // 初始化 SNTP
    ESP_ERROR_CHECK(esp_netif_sntp_init(&config));
}
void wait_for_time_sync(void) {
    int retry = 0;
    const int retry_count = 15; // 最大重试次数

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI("SNTP", "等待时间同步 (%d/%d)...", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (retry == retry_count) {
        ESP_LOGE("SNTP", "时间同步失败!");
    } else {
        time_t now;
        time(&now);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        ESP_LOGI("SNTP", "时间同步成功: %s", asctime(&timeinfo));
    }
}

void send_temp(){
    //配置json 暂时传送电压和温度数据
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root,"temperature",ADC.now_temp);
    cJSON_AddNumberToObject(root,"voltage",system_vol*5);
    cJSON_AddNumberToObject(root,"target_temp",ADC.set_temp);
    cJSON_AddNumberToObject(root,"Kp",pid.Kp);
    cJSON_AddNumberToObject(root,"Ki",pid.Ki);
    cJSON_AddNumberToObject(root,"Kd",pid.Kd);
    cJSON_AddBoolToObject(root,"Sound",Volume);
    cJSON_AddBoolToObject(root,"Heater",en_pid);
    char *json_data = cJSON_PrintUnformatted(root);


   

    esp_http_client_config_t config = {
        .url = "http://192.168.31.71:8080/api/v1/r8ya20c3vpww62ukwea0/telemetry",
        // .url = "https://d17a-113-140-11-126.ngrok-free.app/api/v1/r8ya20c3vpww62ukwea0/telemetry",
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .buffer_size = 1024,
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
        .timeout_ms = 10000,
        .disable_auto_redirect = true,
        .user_agent = "ESP32-HTTP-Client",
        // .cert_pem = server_cert, // 指定证书
        // .skip_cert_common_name_check = false // 严格验证域名
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 设置 HTTP 请求头
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_data, strlen(json_data));

    // 执行请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("HTTP", "HTTP Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE("HTTP", "HTTP Request failed: %s", esp_err_to_name(err));
    }

    // 清理资源
    esp_http_client_cleanup(client);
    cJSON_Delete(root);
    free(json_data);
}

static void get_rpc_command(char *url){
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .timeout_ms = RPC_TIMEOUT_MS,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            ESP_LOGI(TAG, "HTTP GET sucessed, status=%d", status);

        } else {
            ESP_LOGE(TAG, "HTTP GET failed, status=%d", status);
        }
    } else {
        ESP_LOGE(TAG, "HTTP GET error: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
static void fetch_tb_attributes(char *url ){

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = RPC_TIMEOUT_MS , // 略大于长轮询超时
        .disable_auto_redirect = true,
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Accept", "application/json");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            ESP_LOGI(TAG, "HTTP GET sucessed, status=%d", status);

        } else {
            ESP_LOGE(TAG, "HTTP GET failed, status=%d", status);
        }
    } else {
        ESP_LOGE(TAG, "HTTP GET error: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);

}

void http_long_poll_task(void *pvParameters){
    char url[128];
    

    
             while (1) {
                if (wifi_enable){
                    send_temp();
                    snprintf(url, sizeof(url), "http://%s/api/v1/%s/rpc?timeout=%d", THINGSBOARD_SERVER, ACCESS_TOKEN, RPC_TIMEOUT_MS);
                    get_rpc_command(url);
                    snprintf(url, sizeof(url), "http://%s/api/v1/%s/attributes?sharedKeys=Kp,Ki,Kd,temperature&timeout=%d", THINGSBOARD_SERVER, ACCESS_TOKEN, RPC_TIMEOUT_MS);
                    fetch_tb_attributes(url);
                    
                }
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
}
