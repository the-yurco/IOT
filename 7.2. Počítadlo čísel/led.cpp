#include "led.h";
#include "esp_log.h";

const char *Led::TAG = "Led talking: ";

Led::Led(gpio_num_t pin) : m_pin(pin){
    ESP_LOGW(TAG, "Creation of Led class, pin: %d", m_pin);

    gpio_reset_pin(m_pin);
    gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);

    off();
}

Led::~Led(){
    ESP_LOGW(TAG, "Destruction of the Led class, pin: %d", m_pin);
}

void Led::on(){
    ESP_LOGD(TAG, "Switching on ON state, pin: %d", m_pin);
    gpio_set_level(m_pin, 1);
}

void Led::off(){
    ESP_LOGD(TAG, "Switching on OFF state, pin: %d", m_pin);
    gpio_set_level(m_pin, 0);
}

void Led::toggle(){
    if(gpio_get_level(m_pin) == 0){
        ESP_LOGD(TAG, "Switching on ON state, pin: %d", m_pin);
        gpio_set_level(m_pin, 1);
    } else {
        ESP_LOGD(TAG, "Switching on OFF state, pin: %d", m_pin);
        gpio_set_level(m_pin, 0);
    }
}