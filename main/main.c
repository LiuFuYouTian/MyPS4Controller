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

#include "esp_log.h"

#include "ps4.h"
#include "ps4_parser.h"

#define ESP_BD_ADDR_HEX_PTR(addr) \
  (uint8_t*)addr + 0, (uint8_t*)addr + 1, (uint8_t*)addr + 2, \
  (uint8_t*)addr + 3, (uint8_t*)addr + 4, (uint8_t*)addr + 5

#define SPP_TAG "MyPS4Ctrl"

uint8_t PS4_MAC[6] = {0xf0,0xb6,0x1e,0x01,0x57,0x65};

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ps4_control_init(PS4_MAC);

    while (1)
    {
        
        vTaskDelay(1000);

        if (ps4_get_connected_state() == true)
        {
            ESP_LOGE(SPP_TAG, "/////////////////////////////////");
            for (size_t i = 0; i <= PS4_ANALOG_BAT; i++)
            {
                ESP_LOGI(SPP_TAG, "%s length: %d", __func__, ps4_control_get_out(i));
                vTaskDelay(5);
            }
        }
        
    }
}
