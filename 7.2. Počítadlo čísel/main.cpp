#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "headers/display.h"
#include "headers/led.h"
#include "headers/button.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "led.h"
#include <memory>

const gpio_num_t BUTTON_PIN = GPIO_NUM_13;

Display *sevenSegmentDisplay;
Led *led;
Button *button;

extern "C"{
    void app_main(void){
        sevenSegmentDisplay = new Display(GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_23);
        led = new Led(GPIO_NUM_4);
        button = new Button(BUTTON_PIN);

        int presses = 0;
        bool has_been_pressed = false;

        while (1)
        {
            sevenSegmentDisplay->setDigit(presses);

            if (button->isPressed())
            {
                if (!has_been_pressed)
                {
                    if (presses < 15)
                    {
                        presses += 1;
                    }
                    else
                    {
                        presses = 0;
                    }
                    has_been_pressed = true;
                }
            }
            else
            {
                has_been_pressed = false;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        delete sevenSegmentDisplay;
        delete led;
        delete button;
    }
}