#pragma once

#include "driver/gpio.h"

class Display{
public:
    Display(gpio_num_t a, gpio_num_t b, gpio_num_t c, gpio_num_t d, gpio_num_t e, gpio_num_t f, gpio_num_t g);
    ~Display();

    void setDigit(int digit);

private:
    gpio_num_t segments[7];
    static const char *TAG;
};

