idf_component_register(SRCS "Wifi.c"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_netif esp_event
                    INCLUDE_DIRS ".")
