idf_component_register(
    SRCS "ble_beacon_main.c"
    INCLUDE_DIRS ""
    REQUIRES apple_find_my bt nvs_flash esp_partition
)
