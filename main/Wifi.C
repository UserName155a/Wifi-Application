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

#define ESP_WIFI_SSID           "/* WiFi name */"
#define ESP_WIFI_PASSWORD       "/* WiFi password */"
#define MAX_RETRIES             5
#define ESP_WIFI_MAC_ADDRESS    {} /* MAC Address in hexadecimal*/

static const char *TAG = "WiFi.C";
static int retry_number = 0;

void run_on_event(void* handler_arg, 
                  esp_event_base_t event_base, 
                  int32_t event_id, 
                  void* event_data){
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        ESP_ERROR_CHECK(esp_wifi_connect());
        ESP_LOGI(TAG, "Connected");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if (retry_number < MAX_RETRIES){
            ESP_ERROR_CHECK(esp_wifi_connect());
            retry_number++;
        } else {
            ESP_LOGI(TAG, "Connection Failed"); 
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected");
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
            .ssid =                         ESP_WIFI_SSID,
            .password =                     ESP_WIFI_PASSWORD,
            .scan_method =                  WIFI_FAST_SCAN,
            .bssid_set =                    0,
            .bssid =                        ESP_WIFI_MAC_ADDRESS,
            .channel =                      1,
            .listen_interval =              0,
            .sort_method =                  WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold = {
                                            .rssi = -80,
                                            .authmode = WIFI_AUTH_WPA2_WPA3_PSK,
                                            .rssi_5g_adjustment = 0
            },
            .pmf_cfg = {
                                            .capable = true,
                                            .required = false
            },
            .rm_enabled =                   0,
            .btm_enabled =                  0,
            .mbo_enabled =                  0,
            .ft_enabled =                   0,
            .owe_enabled =                  0,
            .transition_disable =           0,
            .disable_wpa3_compatible_mode = 0,
            .reserved1 =                    0,
            .sae_pwe_h2e =                  WPA3_SAE_PWE_BOTH,
            .sae_pk_mode =                  WPA3_SAE_PK_MODE_AUTOMATIC,
            .failure_retry_cnt =            5,
            .he_dcm_set =                   0,
            .he_dcm_max_constellation_tx =  0,
            .he_dcm_max_constellation_rx =  0,
            .he_mcs9_enabled =              0,
            .he_su_beamformee_disabled =    0,
            .he_trig_su_bmforming_feedback_disabled = 0,
            .he_trig_mu_bmforming_partial_feedback_disabled = 0,
            .he_trig_cqi_feedback_disabled =0,
            .vht_su_beamformee_disabled =   0,
            .vht_mu_beamformee_disabled =   1,
            .vht_mcs8_enabled =             0,
            .reserved2 =                    0,
            .sae_h2e_identifier = {0}
        }
    };

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    // 802.11gn protocol
    uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
    esp_wifi_set_protocol(WIFI_IF_STA, protocol);
    esp_wifi_config_11b_rate(WIFI_IF_STA, true); //disables 802.11b
    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW20); 

    ESP_ERROR_CHECK(esp_wifi_start());
}

extern "C" {
    void app_main(void);
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
