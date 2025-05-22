#ifndef BEEP_H
#define BEEP_H
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h" 
#include "esp_log.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (12) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (125) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (2000) // Frequency in Hertz. Set frequency at 2 kHz

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
typedef enum {
    NOTE_C, NOTE_Cs, NOTE_D, NOTE_Eb, NOTE_E, NOTE_F, NOTE_Fs, NOTE_G, NOTE_Gs, NOTE_A, NOTE_Bb, NOTE_B, NOTE_MAX
} note_t;

typedef struct 
{
    note_t note;
    uint8_t rp;
    uint16_t delay;
} TONE;




void beep_init(void);

double GetNote(note_t note, uint8_t rp);
void buzzer_set_freq(uint32_t freq, uint32_t duty_percent);
void set_Tone(uint32_t freq);
void set_Note(note_t note ,uint8_t rp);
void SetSound(TONE sound[]);
void PlaySoundLoop(void);
uint8_t PlayTones(TONE* sound, uint16_t* Schedule);
void beep_test(TONE* soundArray);

extern bool Volume;
extern TONE testSound[];
extern TONE BootSound[];
extern TONE TipInstall[];
extern TONE TipRemove[];
extern TONE Beep1[];
extern TONE Beep2[];
extern TONE Beep3[];
extern TONE twinkleStar[];
extern uint16_t PlayTones_Schedule;
#endif // BEEP_H 