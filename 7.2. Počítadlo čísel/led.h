#pragma once

#include "driver/gpio.h";

class Led{
  public:
    Led(gpio_num_t pin);
    ~Led();

    void on();
    void off();
    void toggle();

  private:
    gpio_num_t m_pin;
    static const char *TAG;
};