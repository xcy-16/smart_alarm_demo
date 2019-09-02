/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_err.h"
#include "esp_event.h"


#include "infra_compat.h"

#include "linkkit_solo.h"
#include "factory_restore.h"
//#include "lightbulb.h"

#include "conn_mgr.h"

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unistd.h>

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <string.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

#include "app_priv.h"
#include "state_define.h"

#include "driver/timer.h"

#include "pwm.h"
#include "alarmtime.h"


#define TME 10

struct Alarmtime alarmtime = {0,0,15};
bool onoff = false;
int state = 0;
esp_timer_handle_t periodic_timer;

int MCOUNT = 14 ;
struct music M[14] = {{DO,0.5},{DO,0.5},{SO,0.5},{SO,0.5},{LA,0.5},{LA,0.5},{SO,1},
                        {FA,0.5},{FA,0.5},{MI,0.5},{MI,0.5},{RE,0.5},{RE,0.5},{DO,0.5}};
struct music START_APP_ALARM[4] = {{DO,0.2},{MI,0.2},{SO,0.2},{1045,0.2}};
struct music FINISH_APP_INIT[6]={{DO,0.5},{EM,0.5},{DO,0.5},{EM,0.5},{DO,0.5},{EM,0.5}};
struct music START_THE_ALARM[2]={{SO,0.2},{DO,0.2}};
struct music RESET_THE_ALARM[2]={{DO,0.2},{SO,0.2}};


void test(void);
static void periodic_timer_callback(void* arg);
static void periodic_back_callback(void* arg);
void timer_alarm_init();

static const char *TAG = "app main";

static bool linkkit_started = false;

static esp_err_t wifi_event_handle(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_GOT_IP:
            if (linkkit_started == false) {
                xTaskCreate((void (*)(void *))linkkit_main, "lightbulb", 10240, NULL, 5, NULL);
                linkkit_started = true;
            }

            break;

        default:
            break;
    }

    return ESP_OK;
}

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            // operate led to indicate user
            ESP_LOGI(TAG, "IOTX_AWSS_START");
            break;

        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            ESP_LOGI(TAG, "IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;

        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            ESP_LOGI(TAG, "IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;

        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            ESP_LOGE(TAG, "IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;

        case IOTX_AWSS_GOT_SSID_PASSWD:
            ESP_LOGI(TAG, "IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
            // discover, router solution)
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
            // (AP and router solution)
            ESP_LOGI(TAG, "IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
            // router.
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
            // ip address
            ESP_LOGI(TAG, "IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;

        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
            // sucess)
            ESP_LOGI(TAG, "IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
            // support bind between user and device
            ESP_LOGI(TAG, "IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
            // user needs to enable awss again to support get ssid & passwd of router
            ESP_LOGW(TAG, "IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD: // Device try to connect cloud
            ESP_LOGI(TAG, "IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
            // net_sockets.h for error code
            ESP_LOGE(TAG, "IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            ESP_LOGI(TAG, "IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;

        case IOTX_RESET: // Linkkit reset success (just got reset response from
            // cloud without any other operation)
            ESP_LOGI(TAG, "IOTX_RESET");
            // operate led to indicate user
            break;

        default:
            break;
    }
}

void app_main()
{
    factory_restore_init();

    conn_mgr_init();
    conn_mgr_register_wifi_event(wifi_event_handle);

    iotx_event_regist_cb(linkkit_event_monitor);    // awss callback

    IOT_SetLogLevel(IOT_LOG_INFO);

    conn_mgr_start();
    test();
}

void test()
{
    light_init();
    app_driver_init();
    pwm_init();
    timer_alarm_init();
    printf("--------------------ALARM INIT FINISH--------------------\n");

    music_start();
    music_play(START_APP_ALARM,4);
    music_end();
    printf("--------------------ALARM APP START--------------------\n");
    printf("Now Timer : %d hours, %d minutes, %d seconds.  \n",alarmtime.hrs,alarmtime.min,alarmtime.sec);

    while(1){
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
    light_off();
}



static void periodic_timer_callback(void* arg)
{
    state = STATE_MUSIC;
    light_off();
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    music_start();
    music_play(M,14);
    music_end();
    state = STATE_MUSIC_END;
    onoff = false;
}


void timer_alarm_init()
{
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

}


