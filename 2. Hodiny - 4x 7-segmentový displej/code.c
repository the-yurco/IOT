#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <esp_task_wdt.h> // Watchdog library handling errors

// Defined GPIO pins
#define COLON_PIN GPIO_NUM_27
#define BTN_1_PIN GPIO_NUM_25
#define BTN_2_PIN GPIO_NUM_26
#define BTN_3_PIN GPIO_NUM_33
#define A1_CONTROL_PIN GPIO_NUM_23
#define A2_CONTROL_PIN GPIO_NUM_13
#define A3_CONTROL_PIN GPIO_NUM_12
#define A4_CONTROL_PIN GPIO_NUM_14
#define DELAY_CLOCK 40

// Declared main constants
const unsigned char sevenSegmentPins[7] = {GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21};
const unsigned char btnPins[3] = {BTN_1_PIN, BTN_2_PIN, BTN_3_PIN};
const unsigned char transistors_diodesPins[5] = {COLON_PIN,  A1_CONTROL_PIN, A2_CONTROL_PIN, A3_CONTROL_PIN, A4_CONTROL_PIN};
const unsigned char patternsSevenSegment[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

// Time variables
uint8_t seconds = 0;
uint8_t minutes = 0;

// Log for debugging
static const char *TAG = "Clock LOG: ";

// Menu, Btn variables
bool timerIsRunning = true;
bool prevBtnState;
bool isMenu = false;
bool isBtnPressed = false;
const uint8_t divider = 10;

// Conf. function
void main_configuration(){
    for (uint8_t i = 0; i < sizeof(btnPins) / sizeof(btnPins[0]); i++){
        gpio_reset_pin(btnPins[i]);
        gpio_set_direction(btnPins[i], GPIO_MODE_INPUT);
        gpio_pullup_en(btnPins[i]);
    }

    for (uint8_t i = 0; i < sizeof(transistors_diodesPins) / sizeof(transistors_diodesPins[0]); i++){
        gpio_reset_pin(transistors_diodesPins[i]);
        gpio_set_direction(transistors_diodesPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(transistors_diodesPins[i], 0);
    }

    for (uint8_t i = 0; i < sizeof(sevenSegmentPins) / sizeof(sevenSegmentPins[0]); i++){
        gpio_reset_pin(sevenSegmentPins[i]);
        gpio_set_direction(sevenSegmentPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(sevenSegmentPins[i], 0);
    }
}

// Function to reset/clear all segments before swtching
void clearSegments(){
    for (uint8_t j = 0; j < 7; j++){
        gpio_set_level(sevenSegmentPins[j], 0);
    }
}

// Functions to extract individiual digits from minutes and seconds
uint8_t getFirstSegmentNumber(){
    return (minutes / divider);
}

uint8_t getSecondSegmentNumber(){
    return minutes % divider;
}

uint8_t getThirdSegmentNumber(){
    return (seconds / divider);
}

uint8_t getFourthSegmentNumber(){
    return seconds % divider;
}

// Function to check Btn state
void checkBtnState(){
    if(gpio_get_level(BTN_3_PIN) == 0){
        if(isBtnPressed){
            prevBtnState = true;
        } else{
            prevBtnState = false;
        }
        isBtnPressed = true;
    }
    else{
        isBtnPressed = false;
        prevBtnState = true;
    }
}

// Function to update time adn handle rollover
void countTime(){
    checkBtnState();
    seconds = (seconds == 59) ? 0 : (seconds + 1);
    minutes = (seconds == 0 && minutes == 59) ? 0 : ((seconds == 0) ? (minutes + 1) : minutes);
}

// Fcuntion to display a digit on the 7-segment display
void displayNumber(uint8_t number, unsigned char segmentControlPin){
    clearSegments();
    gpio_set_level(segmentControlPin, 1);
    for (uint8_t i = 0; i < 7; i++){
        gpio_set_level(sevenSegmentPins[i], patternsSevenSegment[number][i]);
    }
}

// Fnction to display time on the 7-segment displat
void displayTime(){
    uint8_t fourthSegmentNumber = getFourthSegmentNumber();
    uint8_t thirdSegmentNumber = getThirdSegmentNumber();
    uint8_t secondSegmentNumber = getSecondSegmentNumber();
    uint8_t firstSegmentNumber = getFirstSegmentNumber();
    checkBtnState();

    for (uint8_t i = 0; i < 1000 / DELAY_CLOCK; i++){
        if(i == 0){
            gpio_set_level(COLON_PIN, 1);
        } else if (i == (1000 / DELAY_CLOCK) / 2){
            gpio_set_level(COLON_PIN, 0);
        }

        displayNumber(fourthSegmentNumber, A4_CONTROL_PIN);
        vTaskDelay((DELAY_CLOCK / 4) / portTICK_PERIOD_MS);
        gpio_set_level(A4_CONTROL_PIN, 0);

        displayNumber(thirdSegmentNumber, A3_CONTROL_PIN);
        vTaskDelay((DELAY_CLOCK / 4) / portTICK_PERIOD_MS);
        gpio_set_level(A3_CONTROL_PIN, 0);

        displayNumber(secondSegmentNumber, A2_CONTROL_PIN);
        vTaskDelay((DELAY_CLOCK / 4) / portTICK_PERIOD_MS);
        gpio_set_level(A2_CONTROL_PIN, 0);

        displayNumber(firstSegmentNumber, A1_CONTROL_PIN);
        vTaskDelay((DELAY_CLOCK / 4) / portTICK_PERIOD_MS);
        gpio_set_level(A1_CONTROL_PIN, 0);
    }
}

// Function to display the settings menu
void settingsMenu(){
    printf("|=== BOARD SETTINGS ===|\n");
    printf("|   [1] Set Minutes    |\n");
    printf("|   [2] Set Seconds    |\n");
    printf("------------------------\n");
    printf("   [WHITE BTN] - [1] \n");
    printf("   [BLUE BTN] -  [2] \n");
    printf(" [BLACK BTN] - Continue \n");
}

// Function to handle Btn actions, inc. menu navigation
void pressedBtnTreeAction(){
    checkBtnState();
    if(isMenu){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        checkBtnState();

        while(1){
            displayNumber(getFourthSegmentNumber(), A4_CONTROL_PIN);
            vTaskDelay(8 / portTICK_PERIOD_MS);
            gpio_set_level(A4_CONTROL_PIN, 0);
            displayNumber(getThirdSegmentNumber(), A3_CONTROL_PIN);
            vTaskDelay(8 / portTICK_PERIOD_MS);
            gpio_set_level(A3_CONTROL_PIN, 0);
            displayNumber(getSecondSegmentNumber(), A2_CONTROL_PIN);
            vTaskDelay(8 / portTICK_PERIOD_MS);
            gpio_set_level(A2_CONTROL_PIN, 0);
            displayNumber(getFirstSegmentNumber(), A1_CONTROL_PIN);
            vTaskDelay(8 / portTICK_PERIOD_MS);
            gpio_set_level(A1_CONTROL_PIN, 0);

            if (gpio_get_level(BTN_1_PIN) == 0) {
                ESP_LOGI(TAG, "!ACTION: Icreasing minutes: + %d", prevBtnState);
                minutes = (minutes == 59) ? 0 : (minutes + 1);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                return;
            } else if (gpio_get_level(BTN_2_PIN) == 0) {
                ESP_LOGI(TAG, "!ACTION: Icreasing seconds: + %d", prevBtnState);
                seconds = (seconds == 59) ? 0 : (seconds + 1);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                return;
            } else if (gpio_get_level(BTN_3_PIN) == 0 && !isBtnPressed) {
                ESP_LOGI(TAG, "!ACTION: QUITTING:  %d", prevBtnState);
                isMenu = false;
                isBtnPressed = false;
                ESP_LOGI(TAG, "WELCOME %d", prevBtnState);
                vTaskDelay(1500 / portTICK_PERIOD_MS);
                return;
            }
        }
    } else if(isBtnPressed){
        if(!isMenu){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            isBtnPressed = false;
            timerIsRunning = false;
            settingsMenu();
            isMenu = true;
            clearSegments();
            return;
        }
    }
    if (!isMenu && !isBtnPressed) {
        displayTime();
        countTime();
    }
}

// Main function
void app_main(void)
{
    // error handling [watchdog]
    esp_task_wdt_deinit();
    // Conf.
    main_configuration();

    while(1){
        checkBtnState();
        pressedBtnTreeAction();
    }
}