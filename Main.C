#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"

#define ESP_WIFI_SSID       ""    //Wifi name here
#define ESP_WIFI_PASSWORD   ""    //Wifi Password here

static void event_handler(){
  //Code will go here soon
}

void wifi_init(){

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    esp_wifi_init();

    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID;
            .password = ESP_WIFI_PASSWORD;
            .scan_method = WIFI_FAST_SCAN;
            .bssid_set = 0;
            .channel = 1;
        },
    };

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    esp_wifi_start();
}

void app_main(void){
  //No code here for now
}
