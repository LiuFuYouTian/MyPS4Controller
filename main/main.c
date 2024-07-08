#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "esp_mac.h"

#include "ps4.h"
#include "ps4_parser.h"
#include "esp_pwm.h"

#include "esp_log.h"
#define SPP_TAG "MyPS4Ctrl"

esp_pwm_t ESP32_TIME0_PWM = {
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .gpio_num = {GPIO_NUM_18,
                 GPIO_NUM_19,
                 GPIO_NUM_22,
                 GPIO_NUM_23,
                 GPIO_NUM_12,
                 GPIO_NUM_13,
                 GPIO_NUM_26,
                 GPIO_NUM_27},
};

uint8_t PS4_MAC[6] = {0xf0,0xb6,0x1e,0x01,0x57,0x65};

void app_main(void)
{
    uint8_t esp_pwm_ch6 = 0;
    uint8_t esp_pwm_ch7 = 0;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ps4_control_init(PS4_MAC);

    esp_pwm_init(&ESP32_TIME0_PWM);

    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_0,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_1,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_2,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_3,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_4,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_5,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_6,128);
    esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_7,128);

    

    while (1)
    {
        vTaskDelay(15);
        if(ps4_get_connected_state() == true)
        {
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_0,ps4_control_get_out(PS4_ANALOG_ROCKER_RX));
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_1,ps4_control_get_out(PS4_ANALOG_ROCKER_RY));
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_2,ps4_control_get_out(PS4_ANALOG_ROCKER_LX));
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_3,ps4_control_get_out(PS4_ANALOG_ROCKER_LY));
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_4,ps4_control_get_out(PS4_ANALOG_ROCKER_R2));
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_5,ps4_control_get_out(PS4_ANALOG_ROCKER_L2));

            if(ps4_control_get_out(PS4_BUTTON_DIR_UP) != 0) 
                esp_pwm_ch6++;

            if(ps4_control_get_out(PS4_BUTTON_DIR_DOWN) != 0) 
                esp_pwm_ch7++;

            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_6,esp_pwm_ch6);
            esp_pwm_set(&ESP32_TIME0_PWM,LEDC_CHANNEL_7,esp_pwm_ch7);
        }
    }
}
