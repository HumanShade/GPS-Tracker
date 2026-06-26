/*
 * Apple Find My BLE beacon for the Arduino Nano ESP32 (ESP32-S3).
 *
 * Derived from the OpenHaystack / Macless-Haystack ESP32 firmware
 * (https://github.com/dchristl/macless-haystack), licensed under the GNU Affero
 * General Public License v3.0 -- this file is therefore also AGPL-3.0.
 *
 * Changes from upstream:
 *   - targets ESP32-S3 on ESP-IDF v6 (classic-BT release guarded out);
 *   - advertisement address/payload encoding delegated to the host-tested
 *     `apple_find_my` component instead of local set_addr/set_payload helpers.
 *
 * Rolling advertisement keys are stored in the "key" NVS partition (1 count byte
 * followed by N x 28-byte advertisement keys), flashed separately at 0x110000.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_partition.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_random.h"

#include "apple_find_my.h"

/* Advertise a short burst, then deep-sleep this long. Higher = less power, coarser tracking. */
#define DELAY_IN_S 60
/* Reuse one key for this many wake cycles before advancing (60s * 30 = ~30 min/key). */
#define REUSE_CYCLES 30

static const char *LOG_TAG = "macless_haystack";

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/** BLE random static address, derived from the current advertisement key. */
static esp_bd_addr_t rnd_addr;

/** Raw 31-byte Find My advertisement, rebuilt from the current key each cycle. */
static uint8_t adv_data[APPLE_FIND_MY_ADV_LEN];

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min = 0x0020, // 20 ms
    .adv_int_max = 0x0020, // 20 ms
    .adv_type = ADV_TYPE_NONCONN_IND,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

int load_bytes_from_partition(uint8_t *dst, size_t size, int offset)
{
    const esp_partition_t *keypart = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, "key");
    if (keypart == NULL)
    {
        ESP_LOGE(LOG_TAG, "Could not find key partition");
        return 1;
    }
    esp_err_t status = esp_partition_read(keypart, offset, dst, size);
    if (status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Could not read key from partition: %s", esp_err_to_name(status));
    }
    return status;
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&ble_adv_params);
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(LOG_TAG, "advertising start failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(LOG_TAG, "advertising has started.");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(LOG_TAG, "adv stop failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(LOG_TAG, "stop adv successfully");
        }
        break;
    default:
        break;
    }
}

uint8_t get_key_count(void)
{
    uint8_t keyCount[1];
    if (load_bytes_from_partition(keyCount, sizeof(keyCount), 0) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Could not read the key count, stopping.");
        return 0;
    }
    ESP_LOGI(LOG_TAG, "Found %i keys", keyCount[0]);
    return keyCount[0];
}

static uint8_t public_key[APPLE_FIND_MY_KEY_LEN];
RTC_DATA_ATTR uint8_t key_count;
RTC_DATA_ATTR uint8_t key_index;
RTC_DATA_ATTR uint8_t cycle = 0;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
#if CONFIG_IDF_TARGET_ESP32
    /* ESP32-S3 is BLE-only; there is no classic-BT memory to release. */
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
#endif
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&bluedroid_cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED)
    {
        /* Start at a random key index. */
        key_count = get_key_count();
        key_index = (esp_random() % key_count);
        ESP_LOGI(LOG_TAG, "application initialized");
    }

    while (true)
    {
        esp_err_t status;
        int address = 1 + (key_index * sizeof(public_key));
        ESP_LOGI(LOG_TAG, "Loading key with index %d at address %d", key_index, address);
        if (load_bytes_from_partition(public_key, sizeof(public_key), address) != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "Could not read the key, stopping.");
            return;
        }
        ESP_LOGI(LOG_TAG, "using key with start %02x %02x", public_key[0], public_key[1]);

        apple_find_my_set_address(rnd_addr, public_key);
        apple_find_my_build_adv(adv_data, public_key);

        ESP_LOGI(LOG_TAG, "using device address: %02x %02x %02x %02x %02x %02x",
                 rnd_addr[0], rnd_addr[1], rnd_addr[2], rnd_addr[3], rnd_addr[4], rnd_addr[5]);

        if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "gap register error: %s", esp_err_to_name(status));
            return;
        }
        if ((status = esp_ble_gap_set_rand_addr(rnd_addr)) != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "couldn't set random address: %s", esp_err_to_name(status));
            return;
        }
        if ((status = esp_ble_gap_config_adv_data_raw((uint8_t *)&adv_data, sizeof(adv_data))) != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "couldn't configure BLE adv: %s", esp_err_to_name(status));
            return;
        }
        ESP_LOGI(LOG_TAG, "Sending beacon (with key index %d)", key_index);
        vTaskDelay(10);
        esp_ble_gap_stop_advertising();

        if (cycle >= REUSE_CYCLES)
        {
            ESP_LOGI(LOG_TAG, "Max cycles %d reached. Changing key.", cycle);
            key_index = (key_index + 1) % key_count;
            cycle = 0;
        }
        else
        {
            ESP_LOGI(LOG_TAG, "Current cycle is %d. Reusing key.", cycle);
            cycle++;
        }

        ESP_ERROR_CHECK(esp_bluedroid_disable());
        ESP_ERROR_CHECK(esp_bluedroid_deinit());
        ESP_ERROR_CHECK(esp_bt_controller_disable());
        ESP_ERROR_CHECK(esp_bt_controller_deinit());

        vTaskDelay(10);
        ESP_LOGI(LOG_TAG, "Going to sleep");
        vTaskDelay(10);
        esp_sleep_enable_timer_wakeup(DELAY_IN_S * 1000000ULL);
        esp_deep_sleep_start();
    }
}
