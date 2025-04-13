
#include "beep.h"

void beep_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);  // 停止发声
}


// 设置频率和音量（占空比）
void buzzer_set_freq(uint32_t freq, uint32_t duty_percent) {
    // 更新频率
    ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq);

    // 计算占空比（0~8191对应0%~100%）
    uint32_t duty = (8191 * duty_percent) / 100;
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void play_melody(Tone* notes, size_t num_notes) {
    for (size_t i = 0; i < num_notes; i++) {
        if (notes[i].freq > 0) {
            buzzer_set_freq(notes[i].freq, 50);  // 50%音量
        } else {
            ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);  // 停止发声
        }
        vTaskDelay(notes[i].duration_ms / portTICK_PERIOD_MS);
    }
    ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);  // 停止发声
}


