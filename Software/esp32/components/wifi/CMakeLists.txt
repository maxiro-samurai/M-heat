idf_component_register(SRCS  "wifi.c"                         
                       INCLUDE_DIRS "."
                       REQUIRES  esp_wifi lwip esp_http_client json adc heater beep nvs externdraw)