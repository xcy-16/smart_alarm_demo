#ifndef _PWM_H_
#define _PWM_H_

#define EM (1)
#define DO (523)
#define RE (586)
#define MI (658)
#define FA (697)
#define SO (783)
#define LA (879)
#define SI (987)


#define SP {EM,0.5}

struct music
{
    int freq;
    float last;
};


extern struct music M[14];
extern struct music RESET_THE_ALARM[2];

void music_begin(struct music m);
void pwm_init();
void music_play(struct music m[],int num);
void music_start();
void music_end();


#endif