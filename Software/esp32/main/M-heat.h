#ifndef M_heat_H
#define M_heat_H

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "astra_ui_core.h"
#include "astra_ui_draw_driver.h"
#include "astra_ui_item.h"
#include "rotary_encoder.h"
#include "beep.h"
#include "adc.h"
#include "externdraw.h"
#include "firetool_PID_adaptor.h"
#include "heater.h"
#include "nvs_data.h"
#include "timer.h"
#include "wifi.h"

extern bool wifi_enable ;
extern bool ble_state ;



#endif