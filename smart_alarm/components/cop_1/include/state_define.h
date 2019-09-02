
#ifndef _STATE_DEFINE_H_
#define _STATE_DEFINE_H_

#define STATE_WAITING (0)
#define STATE_MUSIC (1)
#define STATE_SET_FINISH (2)
#define STATE_TIMER_ON (3)
#define STATE_TIMER (4)
#define STATE_TIMER_WAIT_ON (5)
#define STATE_TIMER_PAUSE (6)
#define STATE_MUSIC_ON (7)
#define STATE_MUSIC_END (8)
#define STATE_MAX (9)


extern int state;
extern esp_timer_handle_t timer;





#endif