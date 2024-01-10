#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "tcp_client.h"
#include "esp_log.h"
#include "driver/gpio.h"


extern void tcp_client(void);

static const char *TAG = "MAIN";

void app_main(void)
{
    gpio_reset_pin(GPIO_NUM_14);
    gpio_reset_pin(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_14,GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_5,GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_5,GPIO_MODE_INPUT);
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    int sock = 0;
    do
    {
        sock = tcp_client_init("192.168.1.12", 8075);
    } while (sock <= 0);

    bool ledstate = 0;
    while (1)
    {
        // ESP_LOGI(TAG, "Sending hallo...");
        if(gpio_get_level(GPIO_NUM_5) == 1){
            tcp_client_send(sock, "Hallo\n");
        }
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Receiving data from server...%d",gpio_get_level(GPIO_NUM_5));
        ESP_LOGI(TAG, "Receiving data from server...");
        char data_buffer[1000] = {0};
        char key1[] = "LED=ON";
        char key2[] = "LED=OFF";
        char key3[] = "LED=TOOGLE";
        bool rec_status = tcp_client_receive(sock, data_buffer, sizeof(data_buffer));

        if (strcmp (key1,data_buffer) == 0){
            ESP_LOGI(TAG,"LED IS ON");
            ledstate = 1;
        }
        else if (strcmp (key2,data_buffer) == 0){
            ESP_LOGI(TAG,"LED IS OFF");
            ledstate = 0;
        }
        else if (strcmp (key3,data_buffer) == 0){
            ledstate = !ledstate;
            if (ledstate == 0){
                ESP_LOGI(TAG,"LED IS OFF");
            }
            else if (ledstate == 1){
                ESP_LOGI(TAG,"LED IS ON");
            }
            
            
            ESP_LOGI(TAG,"LED IS ON");
        }
        gpio_set_level(GPIO_NUM_14,ledstate);


        // if (rec_status)

        // {
        //     ESP_LOGI(TAG, "Received data - %s", data_buffer);
        // }
        // else
        // {
        //     ESP_LOGW(TAG, "There was some problem during receiving of data");
        // }
    }
}