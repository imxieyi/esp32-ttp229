#include "touchpanel.hpp"
#include "rom/ets_sys.h"
#include "freertos/task.h"
#include "esp_log.h"

static TOUCHPANEL *gpio2tch[48];

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    gpio_num_t gpio_num = (gpio_num_t)(int) arg;
    if(gpio2tch[gpio_num] != NULL)
        gpio2tch[gpio_num] -> interrupt(gpio_num);
}

TOUCHPANEL::TOUCHPANEL(gpio_num_t scl, gpio_num_t sdo) {
    this -> scl = scl;
    this -> sdo = sdo;
    gpio2tch[sdo] = this;
    gpio_config_t conf;
    conf.intr_type = GPIO_INTR_DISABLE;
    conf.mode = GPIO_MODE_OUTPUT;
    conf.pin_bit_mask = 1 << scl;
    conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&conf);
    conf.intr_type = GPIO_INTR_NEGEDGE;
    conf.mode = GPIO_MODE_INPUT;
    conf.pin_bit_mask = 1 << sdo;
    conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&conf);

    gpio_set_level(scl, 1);

    gpio_isr_handler_add(sdo, gpio_isr_handler, (void*)sdo);

    queue = xQueueCreate(10, sizeof(uint16_t));

}

TOUCHPANEL::~TOUCHPANEL() {
    gpio_isr_handler_remove(sdo);
    gpio2tch[sdo] = NULL;
}

void TOUCHPANEL::interrupt(gpio_num_t gpio) {
    if(gpio == sdo) {
        gpio_isr_handler_remove(sdo);
        uint16_t bitmap = 0;
        ets_delay_us(110);
        for(int i = 0; i < 16; i++) {
            gpio_set_level(scl, 0);
            ets_delay_us(50);
            bitmap |= !gpio_get_level(sdo) << i;
            gpio_set_level(scl, 1);
            ets_delay_us(50);
        }
        xQueueSendFromISR(queue, &bitmap, NULL);
        vTaskDelay(2/portTICK_RATE_MS);
        gpio_isr_handler_add(sdo, gpio_isr_handler, (void*)sdo);
    }
}
