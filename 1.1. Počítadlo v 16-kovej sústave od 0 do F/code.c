#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

const gpio_num_t B = GPIO_NUM_4;
const gpio_num_t A = GPIO_NUM_5;
const gpio_num_t C = GPIO_NUM_18;
const gpio_num_t D = GPIO_NUM_19;
const gpio_num_t E = GPIO_NUM_21;
const gpio_num_t F = GPIO_NUM_22;
const gpio_num_t G = GPIO_NUM_23;

const gpio_num_t BUTTON = GPIO_NUM_16;

const uint8_t digitSegments[15][7] = {
    { 1, 1, 1, 1, 1, 1, 0},  //0
    { 0, 1, 1, 0, 0, 0, 0},  //1
    { 1, 1, 0, 1, 1, 0, 1},  //2
    { 1, 1, 1, 1, 0, 0, 1},  //3
    { 0, 1, 1, 0, 0, 1, 1},  //4
    { 1, 0, 1, 1, 0, 1, 1},  //5
    { 1, 0, 1, 1, 1, 1, 1},  //6
    { 1, 1, 1, 0, 0, 0, 0},  //7
    { 1, 1, 1, 1, 1, 1, 1},  //8
    { 1, 1, 1, 1, 0, 1, 1},  //9
    { 1, 1, 1, 0, 1, 1, 1},  //a
    { 0, 0, 1, 1, 1, 1, 1},  //b
    { 1, 0, 0, 1, 1, 1, 0},  //c
    { 0, 1, 1, 1, 1, 0, 1},  //d
    { 1, 0, 0, 1, 1, 1, 1},  //e
    { 1, 0, 0, 0, 1, 1, 1},  //f
};

void board_config()
{
    gpio_num_t segments[7] = {A, B, C, D, E, F, G}; // Local array

    for (int i = 0; i < 7; i++) {
        gpio_reset_pin(segments[i]);
        gpio_set_direction(segments[i], GPIO_MODE_OUTPUT);
    }

    gpio_reset_pin(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
}

bool is_button_pressed()
{
    return (gpio_get_level(BUTTON) == 0);
}

void display(int i){
    gpio_set_level(A,digitSegments[i][0]);
    gpio_set_level(B,digitSegments[i][1]);
    gpio_set_level(C,digitSegments[i][2]);
    gpio_set_level(D,digitSegments[i][3]);
    gpio_set_level(E,digitSegments[i][4]);
    gpio_set_level(F,digitSegments[i][5]);
    gpio_set_level(G,digitSegments[i][6]);
}

void app_main(void)
{
    board_config();

    int presses = 0;
    bool has_been_pressed = false;

    while (1)
    {
        display(presses);

        if(is_button_pressed()){
            if(!has_been_pressed){
                if(presses < 15){
                    presses += 1;
                }else{
                    presses = 0;
                }
                has_been_pressed = true;
            }
        }
        else{
            has_been_pressed = false;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}