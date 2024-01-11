#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

static const char *TAG = "MQTT_EXAMPLE";

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0

bool stavLed = false;
esp_mqtt_client_handle_t client;
bool buttonPressed = false;  



static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void ledc_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000, // Adjust frequency based on your needs
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = GPIO_NUM_4, // Adjust GPIO based on your needs
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_MODE,
        .timer_sel = LEDC_TIMER,
    };
    ledc_channel_config(&ledc_channel);
}

static void process_led(bool new_stav)
{
    if (stavLed != new_stav)
    {
        stavLed = new_stav;
        gpio_set_level(GPIO_NUM_4, stavLed ? 1 : 0);
        if (stavLed)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 255); // full brightness initially
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        }
        else
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0); // turn off LED
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        }
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // Subscribe to the switch and slider topics
        msg_id = esp_mqtt_client_subscribe(client, "/jurco/skuska/switch", 0);
        ESP_LOGI(TAG, "Subscribed to /jurco/skuska/switch, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/jurco/skuska/slider", 0);
        ESP_LOGI(TAG, "Subscribed to /jurco/skuska/slider, msg_id=%d", msg_id);

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        // Check if the message is related to the switch
        if (strncmp("/jurco/skuska/switch", event->topic, event->topic_len) == 0)
        {
            if (strncmp("on", event->data, event->data_len) == 0)
            {
                // Turn on the LED
                process_led(true);
            }
            else if (strncmp("off", event->data, event->data_len) == 0)
            {
                // Turn off the LED
                process_led(false);
            }
        }

        // Check if the message is related to the slider and the LED is ON
        if (stavLed && strncmp("/jurco/skuska/slider", event->topic, event->topic_len) == 0)
        {
            // Process slider value
            uint8_t brightness;
            sscanf(event->data, "%hhu", &brightness);

            // Ensure the brightness is within a valid range (0 to 255)
            brightness = (brightness > 255) ? 255 : brightness;

            // Use brightness to control LED brightness
            // Map the brightness value to the LEDC duty cycle range (0 to 255)
            uint32_t duty = (brightness * 255) / 255; // Adjust the denominator based on the desired maximum brightness

            // Set the LEDC duty cycle
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void button(void *arg)
{
    while (1)
    {
        if (!gpio_get_level(GPIO_NUM_34))
        {
            if (!buttonPressed)
            {
                if (stavLed)
                {
                    stavLed = 0;
                    // Publish "off" to the switch topic
                    esp_mqtt_client_publish(client, "/jurco/skuska/switch", "off", 0, 0, 1);
                }
                else
                {
                    stavLed = 1;
                    // Publish "on" to the switch topic
                    esp_mqtt_client_publish(client, "/jurco/skuska/switch", "on", 0, 0, 1);
                }

                buttonPressed = 1;
            }
        }
        else
        {
            buttonPressed = 0;
        }
    }
}

void app_main(void)
{
    gpio_reset_pin(GPIO_NUM_4);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);

    // Configure switch pin
    gpio_reset_pin(GPIO_NUM_13);
    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLUP_ONLY);

    ledc_init(); // Initialize LEDC

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
    xTaskCreate(button, "button", 2048, NULL, 0, NULL);
}
