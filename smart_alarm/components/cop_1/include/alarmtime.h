#ifndef _ALARMTIME_H_
#define _ALARMTIME_H_
#include <stdbool.h>
#include "esp_timer.h"
#define SECOND (1000000)

struct Alarmtime
{
    int hrs;
    int min;
    int sec;
};

extern struct Alarmtime alarmtime;
extern esp_timer_handle_t periodic_timer;
extern bool onoff;

void Set_Alarmtime(struct Alarmtime target);
void Set_Alarm_On();
void Set_Alarm_Off();

#endif