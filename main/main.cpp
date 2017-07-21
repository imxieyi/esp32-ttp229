#include <cstdio>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touchpanel.hpp"

void touch_task(void *pvParameters) {
	TOUCHPANEL touch(GPIO_NUM_19, GPIO_NUM_22);

	int i;
	uint16_t bitmap;
	while(1) {
		if(xQueueReceive(touch.queue, &bitmap, portMAX_DELAY))
			for(i = 0; i < 16; i++)
				if(bitmap & (1 << i))
					printf("Touch at button %d\n", i+1);
	}
}

extern "C" void app_main() {
	gpio_install_isr_service(0);
	xTaskCreatePinnedToCore(&touch_task, "touchtask", 2048, NULL, 5, NULL, 1);
}
