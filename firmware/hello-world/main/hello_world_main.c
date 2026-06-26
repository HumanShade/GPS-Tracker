#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "banner.h"

static const char *TAG = "tracker";

void app_main(void)
{
    esp_chip_info_t chip;
    esp_chip_info(&chip);

    uint32_t flash_size = 0;
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        flash_size = 0;
    }

    ESP_LOGI(TAG, "Motorcycle tracker - Phase 0 hello-world");
    ESP_LOGI(TAG, "Board: Arduino Nano ESP32 (ESP32-S3 / u-blox NORA-W106)");
    ESP_LOGI(TAG, "Chip: %s, %d core(s), silicon rev v%d.%d",
             CONFIG_IDF_TARGET, chip.cores,
             chip.revision / 100, chip.revision % 100);
    ESP_LOGI(TAG, "Flash: %" PRIu32 " MB, free heap: %" PRIu32 " B",
             flash_size / (1024U * 1024U), esp_get_free_heap_size());

    char uptime[32];
    while (true) {
        banner_format_uptime(uptime, sizeof(uptime), esp_timer_get_time() / 1000);
        ESP_LOGI(TAG, "alive - uptime %s", uptime);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
