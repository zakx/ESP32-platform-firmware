#include <sdkconfig.h>
#include <esp_system.h>
#include <esp_log.h>
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define TAG "dht11"

#define CONFIG_PIN_NUM_DHT11 22
#define TIMEOUT 100000

static esp_err_t dht_gpio_in(void)
{
	esp_err_t res = gpio_set_direction(CONFIG_PIN_NUM_DHT11, GPIO_MODE_INPUT);
	if (res != ESP_OK) return res;
	res = gpio_set_pull_mode(CONFIG_PIN_NUM_DHT11, GPIO_PULLUP_ENABLE);
	return res;
}

static esp_err_t dht_gpio_out(void)
{
	esp_err_t res = gpio_set_direction(CONFIG_PIN_NUM_DHT11, GPIO_MODE_OUTPUT);
	if (res != ESP_OK) return res;
	res = gpio_set_level(CONFIG_PIN_NUM_DHT11, false); //Pull the pin low
	return res;
}

uint32_t dht_pulse(bool level)
{
	uint32_t count = 0;
	while (gpio_get_level(CONFIG_PIN_NUM_DHT11) == level) {
		if (count++ >= 1000) return TIMEOUT;
		ets_delay_us(1);
	}
	return count;
}

esp_err_t dht_read(uint8_t* data)
{
	dht_gpio_out();
	vTaskDelay(20 / portTICK_PERIOD_MS);
	dht_gpio_in();
	ets_delay_us(10);
	if (dht_pulse(false) == TIMEOUT) {
		ESP_LOGE(TAG, "DHT11 pulse 1\n");
		return ESP_FAIL;
	}
	if (dht_pulse(true) == TIMEOUT) {
		ESP_LOGE(TAG, "DHT11 pulse 2\n");
		return ESP_FAIL;
	}
	uint32_t cycles[80];
	for (int i = 0; i < 80; i += 2) {
		cycles[i] = dht_pulse(false);
		cycles[i+1] = dht_pulse(true);
	}
	for (int i = 0; i < 40; i++) {
		uint32_t lowCycles = cycles[2*i];
		uint32_t highCycles = cycles[2*i + 1];
		if ((lowCycles == TIMEOUT) || (highCycles == TIMEOUT)) {
			ESP_LOGE(TAG, "DHT11 pulse i=%d\n", i);
			return ESP_FAIL;
		}
		data[i / 8] <<= 1;
		if (highCycles > lowCycles) {
			// High cycles are greater than 50us low cycle count, must be a 1.
			data[i / 8] |= 1;
		}
	}
	uint8_t chk = data[0]+data[1]+data[2]+data[3];
	//printf("Received from DHT11: %02x %02x %02x %02x  (%02x ? %02x)\n", data[0], data[1], data[2], data[3], data[4], chk);
	if (chk != data[4]) {
		ESP_LOGE(TAG, "DHT11 chksum invalid");
		return ESP_FAIL;
	}
	return ESP_OK;
}

esp_err_t dht11_read(float* temperature, float* humidity)
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00};
	esp_err_t res = dht_read(data);
	if (res != ESP_OK) return res;
	
	*temperature = data[2];
	if (data[3] & 0x80) *temperature = -1 - (*temperature);
	*temperature += (data[3] & 0x0f) * 0.1;
	*humidity = data[0] + data[1] * 0.1;
	return ESP_OK;
}

esp_err_t dht12_read(float* temperature, float* humidity)
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00};
	esp_err_t res = dht_read(data);
	if (res != ESP_OK) return res;
	
	*temperature = data[2] + (data[3] & 0x0f)*0.1;
	if (data[3] & 0x80) *temperature *= -1;
	*humidity = data[0] + data[1] * 0.1;
	return ESP_OK;
}

esp_err_t dht22_read(float* temperature, float* humidity)
{
	uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00};
	esp_err_t res = dht_read(data);
	if (res != ESP_OK) return res;
	
	*temperature = ((word)(data[2] & 0x7F)) << 8 | data[3];
	*temperature *= 0.1;
	if (data[2]&0x80) *temperature *= -1;
	*humidity = ((word)data[0]) << 8 | data[1];
	*humidity *= 0.1;
	return ESP_OK;
}

esp_err_t driver_dht_init(void)
{	
	esp_err_t res = dht_gpio_in();
	if (res != ESP_OK) return res;
	
	/*float temperature, humidity;
	res = dht11_read(&temperature, &humidity);
	if (res != ESP_OK) return res;
	printf("Temperature = %f *c, Humidity = %f %%\n", temperature, humidity);*/
	
	
	return res;
}
