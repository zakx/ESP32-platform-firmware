#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <soc/gpio_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/gpio_struct.h>
#include <soc/spi_reg.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_heap_caps.h"
#include "esp_vfs_fat.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/gpio.h>

#include "driver_vspi.h"
#include "include/driver_sdcard.h"

#ifdef CONFIG_DRIVER_SDCARD_ENABLE

static const char *TAG = "sdcard";

const char* SD_BASE_PATH = "/_#!#_sdcard";

esp_err_t driver_sdcard_init(void)
{	
	static bool driver_sdcard_init_done = false;
	if (driver_sdcard_init_done) return ESP_OK;
	ESP_LOGD(TAG, "init called");
	
	//gpio_set_direction(5, GPIO_MODE_OUTPUT);
	//gpio_set_level(5, 1); //Disable LCD
	
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	host.slot = VSPI_HOST;
	host.max_freq_khz = SDMMC_FREQ_DEFAULT;

	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso   = CONFIG_PIN_NUM_VSPI_MISO;
	slot_config.gpio_mosi   = CONFIG_PIN_NUM_VSPI_MOSI;
	slot_config.gpio_sck    = CONFIG_PIN_NUM_VSPI_CLK;
	slot_config.dma_channel = CONFIG_VSPI_DMA;
	
	slot_config.gpio_cs     = CONFIG_PIN_NUM_SDCARD_CS;

	esp_vfs_fat_sdmmc_mount_config_t mount_config;
	memset(&mount_config, 0, sizeof(mount_config));

	mount_config.format_if_mount_failed = false;
	mount_config.max_files = 3;

	sdmmc_card_t* card;
	esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_BASE_PATH, &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "esp_vfs_fat_sdmmc_mount failed (%d)\n", ret);
		return ret;
	}
	
	driver_sdcard_init_done = true;
	ESP_LOGD(TAG, "init done");
	return ESP_OK;
}

#else
esp_err_t driver_sdcard_init(void) { return ESP_OK; } //Dummy function.
#endif // CONFIG_DRIVER_SDCARD_ENABLE
