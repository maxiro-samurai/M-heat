#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (12) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (2000) // Frequency in Hertz. Set frequency at 5 kHz

typedef struct {
    uint32_t freq;
    uint32_t duration_ms;
} Tone;



void beep_init(void);
void buzzer_set_freq(uint32_t freq, uint32_t duty_percent);
void play_melody(Tone* notes, size_t num_notes);