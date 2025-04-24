/**
 * Writen by yushilong 
 * 参考ESP-IDF的中的NVS例程
 */

#ifndef __NVS_DATA_H__
#define __NVS_DATA_H__

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"

void nvs_init(void);
void nvs_set_parameter(const char* key, int32_t value);
void nvs_get_parameter(const char* key, int32_t *value);
void nvs_clear_parameter(const char* key);
#endif // __NVS_H__