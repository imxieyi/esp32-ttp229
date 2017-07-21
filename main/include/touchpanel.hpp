#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class TOUCHPANEL {
private:
    gpio_num_t scl, sdo; //Not I2C
public:
    xQueueHandle queue;
    TOUCHPANEL(gpio_num_t scl, gpio_num_t sdo);
    ~TOUCHPANEL();
    void interrupt(gpio_num_t gpio);
};
