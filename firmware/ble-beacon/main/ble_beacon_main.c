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

/* After a power-on / normal reset, keep advertising (so the USB-Serial/JTAG stays up and the
 * board is reachable for re-flash and `idf.py monitor`) for this many seconds before entering
 * the deep-sleep cycle. The USB port is unavailable during deep sleep. 0 = sleep immediately. */
#define BEACON_WAKE_WINDOW_S 5
/* If 1, never deep-sleep: advertise continuously. Best for the bench and Apple-network
 * bring-up (more chances to be heard, and USB is always reachable). Set 0 for the low-power
 * production beacon. */
#define BEACON_NO_DEEP_SLEEP 0

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

/* Load advertisement key @p index from the "key" partition and (re)start advertising it. */
static void advertise_key(uint8_t index)
{
    int address = 1 + (index * sizeof(public_key));
    if (load_bytes_from_partition(public_key, sizeof(public_key), address) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Could not read key index %d", index);
        return;
    }
    ESP_LOGI(LOG_TAG, "key index %d (start %02x %02x)", index, public_key[0], public_key[1]);

    apple_find_my_set_address(rnd_addr, public_key);
    apple_find_my_build_adv(adv_data, public_key);
    ESP_LOGI(LOG_TAG, "device address: %02x %02x %02x %02x %02x %02x",
             rnd_addr[0], rnd_addr[1], rnd_addr[2], rnd_addr[3], rnd_addr[4], rnd_addr[5]);

    esp_ble_gap_stop_advertising(); /* harmless before the first start; needed between keys */
    ESP_ERROR_CHECK(esp_ble_gap_set_rand_addr(rnd_addr));
    /* advertising (re)starts in esp_gap_cb on ADV_DATA_RAW_SET_COMPLETE */
    ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw((uint8_t *)&adv_data, sizeof(adv_data)));
}

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
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(esp_gap_cb));

    /* Bitmask is 0 when not woken from deep sleep (i.e. a fresh power-on / normal reset). */
    bool first_boot = (esp_sleep_get_wakeup_causes() == 0);
    if (first_boot)
    {
        key_count = get_key_count();
        ESP_LOGI(LOG_TAG, "application initialized (%d keys)", key_count);
    }

    /* Empty/erased "key" partition reads 0 or 0xFF. Rather than advertise garbage and deep-sleep
     * (which drops USB and makes the board hard to reach), stay awake so a keyfile can be flashed
     * at 0x110000 over USB, then reset. */
    if (key_count == 0 || key_count == 0xFF)
    {
        ESP_LOGE(LOG_TAG, "No keys in 'key' partition. Flash a keyfile at 0x110000 and reset. "
                          "Staying awake (USB reachable).");
        while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }

    if (first_boot)
    {
        key_index = (esp_random() % key_count);
    }

    advertise_key(key_index);

#if BEACON_NO_DEEP_SLEEP
    ESP_LOGI(LOG_TAG, "continuous advertising (no deep sleep)");
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(DELAY_IN_S * 1000));
        key_index = (key_index + 1) % key_count;
        advertise_key(key_index);
    }
#else
    /* Stay awake (USB reachable for re-flash/monitor) on a fresh power-on; just a short burst
     * on subsequent timer wake-ups. */
    int awake_ms = (first_boot && BEACON_WAKE_WINDOW_S > 0) ? (BEACON_WAKE_WINDOW_S * 1000) : 100;
    if (awake_ms > 100)
    {
        ESP_LOGI(LOG_TAG, "advertising; USB reachable for ~%ds before deep sleep", BEACON_WAKE_WINDOW_S);
    }
    vTaskDelay(pdMS_TO_TICKS(awake_ms));
    esp_ble_gap_stop_advertising();

    if (cycle >= REUSE_CYCLES)
    {
        key_index = (key_index + 1) % key_count;
        cycle = 0;
    }
    else
    {
        cycle++;
    }

    ESP_ERROR_CHECK(esp_bluedroid_disable());
    ESP_ERROR_CHECK(esp_bluedroid_deinit());
    ESP_ERROR_CHECK(esp_bt_controller_disable());
    ESP_ERROR_CHECK(esp_bt_controller_deinit());

    ESP_LOGI(LOG_TAG, "deep sleep for %ds", DELAY_IN_S);
    esp_sleep_enable_timer_wakeup(DELAY_IN_S * 1000000ULL);
    esp_deep_sleep_start();
#endif
}
