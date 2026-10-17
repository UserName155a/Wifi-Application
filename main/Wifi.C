#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <esp_netif.h>

#define ESP_WIFI_SSID       "/* WiFi name */"
#define ESP_WIFI_PASSWORD   "/* WiFi password */"
#define MAX_RETRIES         5

static int retry_number = 0;

void run_on_event(void* handler_arg, 
                  esp_event_base_t event_base, 
                  int32_t event_id, 
                  void* event_data){
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        ESP_ERROR_CHECK(esp_wifi_connect());
        ESP_LOGI("Connected");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if (retry_number < MAX_RETRIES){
            ESP_ERROR_CHECK(esp_wifi_connect());
            retry_number++;
        } else {
            ESP_LOGI("Connection Failed"); // these wont be read anyways, I'm trying to do this the hard way
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("Connected");
    }
}

void wifi_start(){

    // init

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

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

    // 802.11gn protocol
    uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
    esp_wifi_set_protocol(WIFI_IF_STA, protocol);
    esp_wifi_config_11b_rate(WIFI_IF_STA, true); //disables 802.11b
    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40); 

    esp_wifi_start();
}

void app_main(void){
    esp_err_t nvs_error = nvs_flash_init();

    if (nvs_error == ESP_ERR_NVS_NO_FREE_PAGES || nvs_error == ESP_ERR_NOT_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_error = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_error);

    wifi_start();
}
