/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <iot_button.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "app_priv.h"

#include "driver/timer.h"
#include "board_esp32_devkitc.h"
#include "state_define.h"
#include "pwm.h"
#include "alarmtime.h"


#define DEBOUNCE_TIME  30

static void push_btn_cb(void *arg)
{
    music_end();
    onoff = false;
    state = STATE_MUSIC_END;
}

static void configure_push_button(int gpio_num, void (*btn_cb)(void *))
{
    button_handle_t btn_handle = iot_button_create(JUMPSTART_BOARD_BUTTON_GPIO, JUMPSTART_BOARD_BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, btn_cb, "RELEASE");
    }
}

void app_driver_init()
{
    configure_push_button(JUMPSTART_BOARD_BUTTON_GPIO, push_btn_cb);
}

void light_init()
{
    gpio_config_t io_conf;
    // Configure output 
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = 1;
    io_conf.pin_bit_mask = GPIO_SEL_27;
    gpio_config(&io_conf);
    printf("light initiated. \n");
}

void light_on()
{
    gpio_set_level(27, 1);
    //printf("light on. \n");
}

void light_off()
{
    gpio_set_level(27, 0);
   // printf("light off. \n");
}