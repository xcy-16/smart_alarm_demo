#include "alarmtime.h"
#include "app_priv.h"
#include "esp_log.h"
#include "pwm.h"
#include "state_define.h"


static const char *TAG = "timer";
void Set_Alarmtime(struct Alarmtime target)
{
    alarmtime = target;
    //flag = 1;
}

void Set_Alarm_On()
{
    state = STATE_WAITING;
    light_on();
    long t = alarmtime.hrs*3600+alarmtime.min*60+alarmtime.sec;
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, t*SECOND));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());
}

void Set_Alarm_Off()
{
    switch(state){
        case(STATE_WAITING):
            light_off();
            ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
            ESP_LOGI(TAG, "Stop timers, time since boot: %lld us", esp_timer_get_time());
            break;
        case(STATE_MUSIC):
            music_end();
            state = STATE_MUSIC_END;
            break;
        default:break;
    }

}