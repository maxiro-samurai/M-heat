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
esp_err_t save_float_to_nvs(nvs_handle_t handle, const char *key, float value);
esp_err_t read_float_from_nvs(nvs_handle_t handle, const char *key, float *value);
#endif // __NVS_H__