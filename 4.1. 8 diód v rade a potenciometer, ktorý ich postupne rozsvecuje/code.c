#define CHANNEL          ADC_CHANNEL_0


#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_11
#define LED1                         13
#define LED2                         12
#define LED3                         14
#define LED4                         27
#define LED5                         25
#define LED6                         32
#define LED7                         33
#define LED8                         26

void reset(){
    gpio_reset_pin(LED1);
    gpio_reset_pin(LED2);
    gpio_reset_pin(LED3);
    gpio_reset_pin(LED4);
    gpio_reset_pin(LED5);
    gpio_reset_pin(LED6);
    gpio_reset_pin(LED7);
    gpio_reset_pin(LED8);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED4, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED5, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED6, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED7, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED8, GPIO_MODE_OUTPUT);
}


void app_main(void)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CHANNEL, &config));

    // config timer for PWM
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    // config pwm
    ledc_channel_config_t pwm_config0 = {
        .gpio_num = GPIO_NUM_5,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
        ledc_channel_config_t pwm_config1 = {
        .gpio_num = GPIO_NUM_18,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
        ledc_channel_config_t pwm_config2 = {
        .gpio_num = GPIO_NUM_19,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
            ledc_channel_config_t pwm_config3 = {
        .gpio_num = GPIO_NUM_21,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_3,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
            ledc_channel_config_t pwm_config4 = {
        .gpio_num = GPIO_NUM_4,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_4,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
            ledc_channel_config_t pwm_config5 = {
        .gpio_num = GPIO_NUM_2,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_5,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
            ledc_channel_config_t pwm_config6 = {
        .gpio_num = GPIO_NUM_14,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_6,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
            ledc_channel_config_t pwm_config7 = {
        .gpio_num = GPIO_NUM_12,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_7,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
    
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config0));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config1));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config2));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config3));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config4));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config5));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config6));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config7));

    while (1) {
        int out = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, CHANNEL, &out));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CHANNEL, out);

        if(out >= 500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, out - 500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
        }
        if(out < 500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
        }
        if(out >= 1000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, out - 1000) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1) );
        }
        if(out < 1000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1) );
        }
        if(out >= 1500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2) );
        }
        if(out < 1500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2) );
        }
        if(out >= 2000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3) );
        }
        if(out < 2000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3) );
        }
        if(out >= 2500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4) );
        }
        if(out < 2500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4) );
        }
        if(out >= 3000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5) );
        }
        if(out <3000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5) );
        }
        if(out >= 3500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_6, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_6) );
        }
        if(out < 3500){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_6, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_6) );
        }
        if(out >= 4000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_7, out - 1500) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_7) );
        }
        if(out < 4000){
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_7, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_7) );
        }
    
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}