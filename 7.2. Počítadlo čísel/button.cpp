#include "button.h";
#include "esp_log.h";

const char *Button::TAG = "Button talking: ";

Button::Button(gpio_num_t pin) : m_pin(pin){
  ESP_LOGW(TAG, "Creation of the Button class, pin: %d", m_pin);

  gpio_reset_pin(m_pin);
  gpio_set_direction(m_pin, GPIO_MODE_INPUT);
  gpio_set_pull_mode(m_pin, GPIO_PULLUP_ONLY);
}

Button::~Button(){
  ESP_LOGW(TAG, "Desctruction of the Button class, pin: %d", m_pin);
}

bool Button::isPressed(){
  return (gpio_get_level(m_pin) == 0);
}