#include "include/nvs.h"
#include "include/platform.h"
#include "include/ota_update.h"

//TEST
#include "driver/gpio.h"
#include <driver/adc.h>
//TEST

extern void micropython_entry(void);

extern esp_err_t unpack_first_boot_zip(void);

/*
#define ODROID_GAMEPAD_IO_X ADC1_CHANNEL_6
#define ODROID_GAMEPAD_IO_Y ADC1_CHANNEL_7
#define ODROID_GAMEPAD_IO_SELECT GPIO_NUM_27
#define ODROID_GAMEPAD_IO_START GPIO_NUM_39
#define ODROID_GAMEPAD_IO_A GPIO_NUM_32
#define ODROID_GAMEPAD_IO_B GPIO_NUM_33
#define ODROID_GAMEPAD_IO_MENU GPIO_NUM_13
#define ODROID_GAMEPAD_IO_VOLUME GPIO_NUM_0
void inputTest()
{
	gpio_set_direction(ODROID_GAMEPAD_IO_SELECT, GPIO_MODE_INPUT);
	gpio_set_pull_mode(ODROID_GAMEPAD_IO_SELECT, GPIO_PULLUP_ONLY);
	gpio_set_direction(ODROID_GAMEPAD_IO_START, GPIO_MODE_INPUT);
	gpio_set_direction(ODROID_GAMEPAD_IO_A, GPIO_MODE_INPUT);
	gpio_set_pull_mode(ODROID_GAMEPAD_IO_A, GPIO_PULLUP_ONLY);
    gpio_set_direction(ODROID_GAMEPAD_IO_B, GPIO_MODE_INPUT);
	gpio_set_pull_mode(ODROID_GAMEPAD_IO_B, GPIO_PULLUP_ONLY);
	
	adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ODROID_GAMEPAD_IO_X, ADC_ATTEN_11db);
	adc1_config_channel_atten(ODROID_GAMEPAD_IO_Y, ADC_ATTEN_11db);

	gpio_set_direction(ODROID_GAMEPAD_IO_MENU, GPIO_MODE_INPUT);
	gpio_set_pull_mode(ODROID_GAMEPAD_IO_MENU, GPIO_PULLUP_ONLY);

	gpio_set_direction(ODROID_GAMEPAD_IO_VOLUME, GPIO_MODE_INPUT);

	while (1) {
		int joyX = adc1_get_raw(ODROID_GAMEPAD_IO_X);
		int joyY = adc1_get_raw(ODROID_GAMEPAD_IO_Y);
		bool btnA = !(gpio_get_level(ODROID_GAMEPAD_IO_A));
		bool btnB = !(gpio_get_level(ODROID_GAMEPAD_IO_B));
		bool btnMenu = !(gpio_get_level(ODROID_GAMEPAD_IO_MENU));
		bool btnVolume = !(gpio_get_level(ODROID_GAMEPAD_IO_VOLUME));
		bool btnSelect = !(gpio_get_level(ODROID_GAMEPAD_IO_SELECT));
		bool btnStart = !(gpio_get_level(ODROID_GAMEPAD_IO_START));
		//printf("%06d / %06d", joyX, joyY);
		
		bool joyUp    = (joyY > 2048 + 1024);
		bool joyDown  = (!joyUp) & (joyY > 1024);
		bool joyLeft  = (joyX > 2048 + 1024);
		bool joyRight = (!joyLeft) & (joyX > 1024);
		
		if (joyUp) {
			if (joyLeft) {
				printf("LEFT-UP");
			} else if (joyRight) {
				printf("RIGHT-UP");
			} else {
				printf("UP");
			}
		} else if (joyDown) {
			if (joyLeft) {
				printf("LEFT-DOWN");
			} else if (joyRight) {
				printf("RIGHT-DOWN");
			} else {
				printf("DOWN");
			}
		} else {
			if (joyLeft) {
				printf("LEFT");
			} else if (joyRight) {
				printf("RIGHT");
			} else {
				printf("NONE");
			}
		}
		
		if (btnA) printf(" / A");
		if (btnB) printf(" / B");
		if (btnMenu) printf(" / MENU");
		if (btnVolume) printf(" / VOLUME");
		if (btnSelect) printf(" / SELECT");
		if (btnStart) printf(" / START");
		printf("\n");
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}
}
*/

void app_main()
{
	logo();
	bool is_first_boot = nvs_init();
	platform_init();

	if (is_first_boot) {
		printf("Attempting to unpack FAT initialization ZIP file...\b");
		if (unpack_first_boot_zip() != ESP_OK) {
			printf("An error occured while unpackint the ZIP file!\b");
			restart();
		}
	}
	
	int magic = get_magic();
	
	switch(magic) {
		case MAGIC_OTA:
			badge_ota_update();
			break;
		default:
			micropython_entry();
	}
	
	inputTest();
}
