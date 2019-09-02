/* MCPWM basic config example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use each submodule of MCPWM unit.
 * The example can't be used without modifying the code first.
 * Edit the macros at the top of mcpwm_example_basic_config.c to enable/disable the submodules which are used in the example.
 */

#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "soc/rtc.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "pwm.h"




#define GPIO_PWM0A_OUT 19   //Set GPIO 19 as PWM0A

#define GPIO_SYNC0_IN   2   //Set GPIO 02 as SYNC0
#define GPIO_SYNC1_IN   4   //Set GPIO 04 as SYNC1
#define GPIO_SYNC2_IN   5   //Set GPIO 05 as SYNC2
#define GPIO_FAULT0_IN 32   //Set GPIO 32 as FAULT0
#define GPIO_FAULT1_IN 34   //Set GPIO 34 as FAULT1
#define GPIO_FAULT2_IN 34   //Set GPIO 34 as FAULT2



static void mcpwm_example_gpio_initialize()
{
    //printf("initializing mcpwm gpio...\n");

    mcpwm_pin_config_t pin_config = {
        .mcpwm0a_out_num = GPIO_PWM0A_OUT,
        
    };
    mcpwm_set_pin(MCPWM_UNIT_0, &pin_config);

}


/**
 * @brief Configure whole MCPWM module
 */
void pwm_init()
{
    //1. mcpwm gpio initialization
    mcpwm_example_gpio_initialize();

    //2. initialize mcpwm configuration
   // printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    //frequency = 1000Hz
    pwm_config.cmpr_a = 50.0;       //duty cycle of PWMxA = 60.0%
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);   //Configure PWM0A & PWM0B with above settings
    music_end();
}
void music_start()
{
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
}

void music_play(struct music m[],int num)
{
    int i = 0;
    while(i<num)
    {
        mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0,m[i].freq);
        vTaskDelay(m[i].last * 1000 / portTICK_PERIOD_MS);
        i+=1;
    }
    
}

void music_begin(struct music m)
{
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0,m.freq);
    vTaskDelay(m.last * 1000 / portTICK_PERIOD_MS);
}



void music_end()
{
    mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
}
/*
void app_main()
{
    printf("Testing MCPWM...\n");
    //cap_queue = xQueueCreate(1, sizeof(capture)); //comment if you don't want to use capture module
    //xTaskCreate(disp_captured_signal, "mcpwm_config", 4096, NULL, 5, NULL);  //comment if you don't want to use capture module
    //xTaskCreate(gpio_test_signal, "gpio_test_signal", 4096, NULL, 5, NULL); //comment if you don't want to use capture module
    xTaskCreate(mcpwm_example_config, "mcpwm_example_config", 4096, NULL, 5, NULL);
}
 */
