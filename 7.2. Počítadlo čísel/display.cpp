#include "display.h"
#include "digitSegments.h";

Display::Display(gpio_num_t a, gpio_num_t b, gpio_num_t c, gpio_num_t d, gpio_num_t e, gpio_num_t f, gpio_num_t g){
    // segments[0] = a;
    // segments[1] = b;
    // segments[2] = c;
    // segments[3] = d;
    // segments[4] = e;
    // segments[5] = f;
    // segments[6] = g;
    gpio_num_t segmentPins[7] = {a, b, c, d, e, f, g};

    for (int i = 0; i < 7; i++)
    {
        segments[i] = segmentPins[i];
        
        gpio_reset_pin(segments[i]);
        gpio_set_direction(segments[i], GPIO_MODE_OUTPUT);
    }
}

Display::~Display(){
}

void Display::setDigit(int digit){
    gpio_set_level(segments[0], digitSegments[digit][0]);
    gpio_set_level(segments[1], digitSegments[digit][1]);
    gpio_set_level(segments[2], digitSegments[digit][2]);
    gpio_set_level(segments[3], digitSegments[digit][3]);
    gpio_set_level(segments[4], digitSegments[digit][4]);
    gpio_set_level(segments[5], digitSegments[digit][5]);
    gpio_set_level(segments[6], digitSegments[digit][6]);
}
