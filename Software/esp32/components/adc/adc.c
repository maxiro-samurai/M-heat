#include "adc.h"


static adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
static adc_continuous_handle_t handle = NULL;
static TaskHandle_t s_task_handle;
static const char *TAG = "EXAMPLE";
adc_continuous_item ADC = {
    .adc_buffer_1 = {0},
    .adc_buffer_2 = {0},
    .vol_low = 0,
    .vol_high = 0,
    .adc_error = 0,
    .now_temp = 0,
    .now_temp_high = 0,
    .adc_get_temp_flg = false,
    .hotbed_max_temp = 0,
    .adc_max_temp = 250,
    .adc_max_temp_auto_flg = true,
};
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;     // 持续读取ADC事件句柄

    adc_continuous_handle_cfg_t adc_config = {  //缓冲区配置
        .max_store_buf_size = 1024,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = EXAMPLE_ADC_CONV_MODE,    //单通道转换模式
        .format = EXAMPLE_ADC_OUTPUT_TYPE,     //DMA输出格式
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = EXAMPLE_ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = EXAMPLE_ADC_UNIT;
        adc_pattern[i].bit_width = EXAMPLE_ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}


static void adc_get_value(adc_continuous_item *adc) {

    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};
    esp_err_t ret;
    uint8_t m = 0;
    uint8_t n = 0;
    memset(result, 0xcc, EXAMPLE_READ_LEN);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待通知

    while (1) {
        ret = adc_continuous_read(handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
        if (ret == ESP_OK) {
            ESP_LOGI("TASK", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
            for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                adc_digi_output_data_t *p = (void*)&result[i];
                uint32_t chan_num = EXAMPLE_ADC_GET_CHANNEL(p);
                uint32_t data = EXAMPLE_ADC_GET_DATA(p);
                
                if (chan_num == ADC_CHANNEL_6 ){
                    // ESP_LOGI(TAG, "ADC_CHANNEL_6 data is :%lu", data);
                    adc->adc_buffer_1[m] = data; //存储ADC值
                    
                    m++;
                    if (m >= 8) {
                        m = 0;
                    }
                } else if (chan_num == ADC_CHANNEL_7 )
                {
                    // ESP_LOGI(TAG, "ADC_CHANNEL_7 data is :%lu", data);
                    adc->adc_buffer_2[n] = data; //存储ADC值
                    n++;
                    if (n >= 8) {
                        n = 0;
                    }
                }
            }
            //  vTaskDelay(1);
            } else if (ret == ESP_ERR_TIMEOUT) {
            
                break;
            }
        }

    }


static void adc_voltage(adc_continuous_item *adc)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        adc->vol_low += adc->adc_buffer_1[i]; //低电平ADC值
        adc->vol_high += adc->adc_buffer_2[i]; //高电平ADC值
    }
    adc->vol_low >>= 3; //平均值滤波
    adc->vol_high >>= 3; //

    adc->vol_low = adc->vol_low * 1000 / 4095; //转换为电压值
    adc->vol_high = adc->vol_high * 1000 / 4095; //转换为电压值
    ESP_LOGI(TAG, "vol_low is :%lu", adc->vol_low);
    ESP_LOGI(TAG, "vol_high is :%lu", adc->vol_high);
}



static void calculate_temperature(adc_continuous_item *adc){

     // 1. 计算热敏电阻的电压值 (单位：mV)
     double voltage = (double) adc->vol_low* 1100 / 4095;
     ESP_LOGI(TAG, "voltage is :%f", voltage);
     // 2. 计算热敏电阻的电阻值
     double resistance = 130000 * (1100 / voltage - 1);
     ESP_LOGI(TAG, "resistance is :%f", resistance);
     // 3. 使用 Beta 参数公式计算温度 (单位：K)
     double temperature_kelvin = 1 / (1 / 298.15 + log(resistance / 130000) / 3950);
 
     // 4. 转换为摄氏温度
     double temperature_celsius = temperature_kelvin - 273.15;
    ESP_LOGI(TAG, "temperature_celsius is :%f", temperature_celsius);
     
}






static void adc_temp(adc_continuous_item *adc){

    int32_t rt;
    int32_t tt;
    int32_t temp_buf = 0;
    double buf;
    

    rt = adc->vol_low * 1000 / ((1100 - adc->vol_low) / 13);
    adc->now_temp = (100 / (log(rt / 10000.0) / 3950 + 1 / 298.15) - 27315) / 100; //转换为温度值
    ESP_LOGI(TAG, "now_temp is :%u", adc->now_temp);
    // if (adc->now_temp < 151)
    //     return;

    // if (adc->now_temp < 160 && !adc->adc_error)
    // {
    //     // if (pwm.power || adc_max_temp_auto_flg == 0)
    //     if (adc->adc_max_temp_auto_flg == 0)
    //         temp_buf = adc->now_temp;
    // }

    // adc->vol_high = adc->vol_high * 1000 / 21;

    // buf = adc->vol_high * 1000 / ((3300000 - adc->vol_high) / 13.0);
    // tt = (100 / (log(buf / 10000) / 3950 + 1 / 298.15) - 27315) / 100;
    // // ESP_LOGI(TAG, "tt is :%u", tt);
    

    // if (temp_buf)
    //     adc->adc_error = temp_buf - tt;

    // if (adc->adc_max_temp_auto_flg)
    // {
    //     int16_t tmp = 150 - adc->adc_error;
    //     rt = adc->hotbed_max_temp - adc->adc_error;
    //     int16_t tmp1 = adc->adc_max_temp - rt;
    //     buf = (double)(tmp1) / (double)(adc->adc_max_temp - tmp);
    //     tt = (double)tt - ((double)(tt - tmp) * buf);
    // }
    // if (adc->adc_max_temp_auto_flg)
    //     adc->now_temp = tt + adc->adc_error;
    // else
    //     adc->now_temp = tt;
   
}

void adc_continuous_read_task(void *arg)
{
    
    // memset(result, 0xcc, EXAMPLE_READ_LEN);

    s_task_handle = xTaskGetCurrentTaskHandle(); //获取任务句柄

    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle); //初始化ADC

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };   

    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));//注册中断回调函数
    ESP_ERROR_CHECK(adc_continuous_start(handle));  //启动ADC

    while (1)
    {
        adc_get_value(&ADC); //获取ADC值
        adc_voltage(&ADC); //获取电压值
        // calculate_temperature(&ADC); //计算温度值
        adc_temp(&ADC); //获取温度值
        // adc_voltage(&ADC);
        vTaskDelay(1000); //延时1ms
        
    }
    
}