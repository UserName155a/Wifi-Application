idf_component_register(SRCS "Wifi.C"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_netif esp_event
                    INCLUDE_DIRS ".")
