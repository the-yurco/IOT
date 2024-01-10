#pragma once

#include "driver/gpio.h";

class Button{
  public:
    Button(gpio_num_t pin);
    ~Button();

    bool isPressed();

  private:
    gpio_num_t m_pin;
    static const char *TAG;
};