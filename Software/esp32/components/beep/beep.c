
#include "beep.h"
bool Volume = true;
TONE testSound[]{
    {NOTE_D,   CMT_9,    250},
    {NOTE_D,   CMT_7,    250},
    {NOTE_D,   CMT_5,    250},
    {NOTE_D,   CMT_9,    250},
    {NOTE_D,   CMT_8,    250},
    {NOTE_D,   CMT_7,    250},
    {NOTE_D,   CMT_NULL, 250},

    {NOTE_D,   CMT_7,    250},
    {NOTE_D,   CMT_5,    250},
    {NOTE_D,   CMT_9,    250},
    {NOTE_D,   CMT_4,    250},
    {NOTE_D,   CMT_5,    250},
    {NOTE_D,   CMT_NULL, 250},

    {NOTE_D,   CMT_7,    250},
    {NOTE_D,   CMT_5,    250},
    {NOTE_D,   CMT_9,    250},
    {NOTE_D,   CMT_7,    250},
    {NOTE_D,   CMT_M,    250},
    {NOTE_D,   CMT_NULL, 250},

    {NOTE_MAX, 255,      0},
};

TONE BootSound[]{
    {NOTE_D,   CMT_5, 230},
    {NOTE_D,   CMT_7, 230},
    {NOTE_D,   CMT_9, 215},
    {NOTE_D,   CMT_M, 215},
    {NOTE_MAX, 0,     0},
};

TONE TipInstall[]{
    {NOTE_D,   CMT_7, 250},
    {NOTE_D,   CMT_M, 250},
    {NOTE_MAX, 0,     0},
};

TONE TipRemove[]{
    {NOTE_D,   CMT_9, 250},
    {NOTE_D,   CMT_5, 250},
    {NOTE_MAX, 0,     0},
};

TONE Beep1[]{
    {NOTE_D,   CMT_8, 50},
    {NOTE_MAX, 0,     0},
};

TONE Beep2[]{
    {NOTE_D,   CMT_M,    50},
    {NOTE_D,   CMT_NULL, 50},
    {NOTE_D,   CMT_M,    50},
    {NOTE_MAX, 0,        0},
};

TONE Beep3[]{
    {NOTE_D,   CMT_7, 50},
    {NOTE_D,   CMT_9, 50},
    {NOTE_D,   CMT_M, 50},
    {NOTE_MAX, 0,     0},
};

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

/*** 
 * @description: 发出指定音符的音调
 * @param {note_t} note 绝对音高
 * @param {uint8_t} rp 相对音高（全音）
 * @return {*}
 * https://www.zhihu.com/question/27166538/answer/35507740
 * 王赟 Maigo ：
 * 音名是用字母CDEFGAB表示的，它表示的是绝对音高。一个八度中有12个音，分别是C, #C, D, #D, E, F, #F, G, #G, A, #A, B，相邻两个音之间的距离叫一个半音，两个半音叫一个全音。在最常见的定律法——十二平均律中，中央C（钢琴最中间的C音）右边的第一个A音被定义为440 Hz，然后其它音的频率用等比数列推出，相隔半音的两个音的频率之比为pow(2,1/12.0)。由此可以推断出中央C的频率为261 Hz，它右边的#C的频率为277 Hz，D的频率为293 Hz，等等等等。
唱名指的是do, re, mi这些，可以用数字1234567表示。一个八度中也有12个音，分别是1, #1, 2, #2, 3, 4, #4, 5, #5, 6, #6, 7，相邻两个音之间的距离也是半音。一首曲子是什么调，就是说哪个音名对应着唱名1。所以，唱名表示的是相对音高。比如C调就是说C音是1，这样1的频率就是261 Hz，#1的频率就是277 Hz，2的频率就是293 Hz，等等。而在#C调中，就变成了1是277 Hz，#1是293 Hz，等等。调式总共有12种，你可以按上述方法自己推算。
 */

 double GetNote(note_t note, uint8_t rp)
 {
     const uint16_t noteFrequencyBase[12] = {
         //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
         4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902
     };
     const uint8_t octave = 4;
     const double FreqRatio = 1.059463094; // pow(2,1/12.0);
     const double FreqRatioDouble = 1.122462048; // pow(2,1/6.0);
     double noteFreq = ((double) noteFrequencyBase[note] / (double) (1 << (8 - octave))) * pow(FreqRatio, rp);
     //SetTone(noteFreq);
     return noteFreq;
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

void set_Tone(uint32_t freq)
{
    buzzer_set_freq(freq,50);

}

void set_Note(note_t note ,uint8_t rp)
{
    set_Tone(GetNote(note,rp));
}

void SetSound(TONE sound[])
{
    MySound = sound;
    PlayTones_Schedule = 0;
}

void ICACHE_RAM_ATTR PlaySoundLoop(void)
{
    if (!Volume)
    {
        MySound = NULL;
        SetTone(0);
    }
    if (MySound == NULL) return;

    PlayTones(MySound, &PlayTones_Schedule);
}

uint8_t PlayTones(TONE* sound, uint16_t* Schedule)
{
    if (xTaskGetTickCount() * portTICK_PERIOD_MS - PlayTonesTimer > PlayTonesDelayTime)
    {

        if (sound == NULL || Schedule == NULL || sound[*Schedule].note == NOTE_MAX || !Volume)
        {
            if (Volume && sound[*Schedule].rp == 255)
            {
                *Schedule = 0; //循环播放
            } else
            {
                SetTone(0);
                return 1;
            }
        }

        //下一个音符
        if (sound[*Schedule].rp == CMT_NULL) SetTone(0);
        else SetTone(GetNote(sound[*Schedule].note, sound[*Schedule].rp));

        PlayTonesTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        //设置延时时间
        PlayTonesDelayTime = sound[*Schedule].delay;
        (*Schedule)++;
    }

    return 0;
}