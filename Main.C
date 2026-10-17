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


static int retry_number = 0;

void run_on_event(void* handler_arg, 
                  esp_event_base_t event_base, 
                  int32_t event_id, 
                  void* event_data){
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();
        ESP_LOGI("Connected");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if (retry_number < MAX_RETRIES){
            esp_wifi_connect();
            retry_number++;
        } else {
            ESP_LOGI("Connection Failed");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("Connected");
    }
}

void wifi_start(){

    // init

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    esp_wifi_init(&wifi_init_config);

    // config

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

    esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G);
    esp_wifi_config_11b_rate(ifx, true) 

    esp_wifi_start();
}

void app_main(void){
    nvs_flash_init();

    wifi_start();
}
