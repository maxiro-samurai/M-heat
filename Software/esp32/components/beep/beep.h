#ifndef BEEP_H
#define BEEP_H
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

enum CALCULATORMUSICTONE
{
    CMT_NULL = 255,
    CMT_1 = 0,
    CMT_2 = 2,
    CMT_3 = 3,
    CMT_4 = 5,
    CMT_5 = 7,
    CMT_6 = 9,
    CMT_7 = 11,
    CMT_8 = 12,
    CMT_9 = 14,
    CMT_P = 15,
    CMT_S = 17,
    CMT_M = 18,
    CMT_D = 21,
    CMT_E = 22,
};
struct TONE
{
    note_t note;
    uint8_t rp;
    uint16_t delay;
};
typedef enum {
    NOTE_C, NOTE_Cs, NOTE_D, NOTE_Eb, NOTE_E, NOTE_F, NOTE_Fs, NOTE_G, NOTE_Gs, NOTE_A, NOTE_Bb, NOTE_B, NOTE_MAX
} note_t;



void beep_init(void);
void buzzer_set_freq(uint32_t freq, uint32_t duty_percent);

extern bool Volume;
#endif // BEEP_H 