#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

// Define pins
#define SEG_A GPIO_NUM_5
#define SEG_B GPIO_NUM_4
#define SEG_C GPIO_NUM_18
#define SEG_D GPIO_NUM_19
#define SEG_E GPIO_NUM_21
#define SEG_F GPIO_NUM_22
#define SEG_G GPIO_NUM_23

// Define GPIO pin for the button
#define BUTTON GPIO_NUM_16

// NUM/LETT. patterns for 7-segment-display
const uint8_t digitSegments[7][7] = {
    {1, 1, 1, 1, 1, 1, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1},  // 2
    {1, 1, 1, 1, 0, 0, 1},  // 3
    {0, 1, 1, 0, 0, 1, 1},  // 4
    {1, 0, 1, 1, 0, 1, 1},  // 5
    {1, 0, 1, 1, 1, 1, 1},  // 6
    // {1, 1, 1, 0, 0, 0, 0},  // 7
    // {1, 1, 1, 1, 1, 1, 1},  // 8
    // {1, 1, 1, 1, 0, 1, 1},  // 9
    // {1, 1, 1, 0, 1, 1, 1},  // A
    // {0, 0, 1, 1, 1, 1, 1},  // B
    // {1, 0, 0, 1, 1, 1, 0},  // C
    // {0, 1, 1, 1, 1, 0, 1},  // D
    // {1, 0, 0, 1, 1, 1, 1},  // E
    // {1, 0, 0, 0, 1, 0, 0}   // F
};

const gpio_num_t segments[7] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

void board_config()
{
    for (int i = 0; i < 7; i++) {
        gpio_reset_pin(segments[i]);
        gpio_set_direction(segments[i], GPIO_MODE_OUTPUT);
    }

    gpio_reset_pin(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
}

// Function to check if the button is pressed
bool is_button_pressed()
{
    return (gpio_get_level(BUTTON) == 0);
}

// Function to generate a random digit or letter
int get_random_digit(){
    return rand() % 16;
}

// Function to display a digit or letter on the 7-segment display
void display(int i)
{
    for (int j = 0; j < 7; j++) {
        gpio_set_level(segments[j], digitSegments[i][j]);
    }
}

void app_main(void){
    board_config();

    int last_presses = -1;
    bool button_was_pressed = false;

    while (1)
    {
        if (is_button_pressed())
        {
            // Only change the display once on button press
            if (!button_was_pressed)
            {

                // Check if the generated digit is different from the last one
                int presses = get_random_digit();
                if (presses != last_presses)

                {
                    display(presses);
                    last_presses = presses;
                }

                // Set the flag to indicate that the button has been pressed
                button_was_pressed = true;
            }
        }
        else
        {
            // Reset the button press flag when the button is released
            button_was_pressed = false;
        }
    }
}