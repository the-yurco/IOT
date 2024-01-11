#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#define PORT CONFIG_EXAMPLE_PORT
#define LED_COUNT 8

const gpio_num_t LED1 = GPIO_NUM_15;
const gpio_num_t LED2 = GPIO_NUM_23;
const gpio_num_t LED3 = GPIO_NUM_4;
const gpio_num_t LED4 = GPIO_NUM_5;
const gpio_num_t LED5 = GPIO_NUM_18;
const gpio_num_t LED6 = GPIO_NUM_19;
const gpio_num_t LED7 = GPIO_NUM_21;
const gpio_num_t LED8 = GPIO_NUM_22;

gpio_num_t leds[8] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8};


char strings[8][20];
/*
    strcpy(strings[0], "STATE=1");
    strcpy(strings[1], "STATE=2");
    strcpy(strings[2], "STATE=3");
*/
void ledsInit() // toto pojde na server
{
    for (int i = 0; i < 8; i++)
    {
        gpio_reset_pin(leds[i]);
        gpio_set_direction(leds[i], GPIO_MODE_OUTPUT);
        gpio_set_level(leds[i], 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
    gpio_reset_pin(GPIO_NUM_2);
    gpio_set_level(GPIO_NUM_2, 0);
}

void led1(){
    for(int i = 0; i < 8; i++){
        if(i == 0){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led2(){
    for(int i = 0; i < 8; i++){
        if(i <= 1){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led3(){
    for(int i = 0; i < 8; i++){
        if(i <= 2){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led4(){
    for(int i = 0; i < 8; i++){
        if(i <= 3){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led5(){
    for(int i = 0; i < 8; i++){
        if(i <= 4){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led6(){
    for(int i = 0; i < 8; i++){
        if(i <= 5){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led7(){
    for(int i = 0; i < 8; i++){
        if(i <= 6){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
void led8(){
    for(int i = 0; i < 8; i++){
        if(i <= 7){
            gpio_set_level(leds[i], 1);
        }
        else{
            gpio_set_level(leds[i], 0);
        }
    }
}
static const char *TAG = "example"; // example pointer na terminal output

char myString[] = "Hello, World!";

static void do_retransmit(const int sock)
{
    int len;             // pocet bytov dat // nepodstatne
    char rx_buffer[128]; // velkost buffera
    ledsInit();
    

    do
    {
        
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0); // receive funkcia na socket
        if (len < 0)
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0;
            
            ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer); // toto sa vypisuje do terminalu, je to len size spravy
            int value = atoi(rx_buffer);
            ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

            if (strcmp(rx_buffer, "STATE=0") == 0)
            {
                for(int i = 0; i < 8; i++){
                    gpio_set_level(leds[i], 0);
                }
                
            }
            if (strcmp(rx_buffer, "STATE=1") == 0)
            {
                 led1();
                
            }
            if (strcmp(rx_buffer, "STATE=2") == 0)
            {
                 led2();
            }
            if (strcmp(rx_buffer, "STATE=3") == 0)
            {
                 led3();
            }
            if (strcmp(rx_buffer, "STATE=4") == 0)
            {
                 led4();
            }
            if (strcmp(rx_buffer, "STATE=5") == 0)
            {
                 led5();
            }
            if (strcmp(rx_buffer, "STATE=6") == 0)
            {
                 led6();
            }
            if (strcmp(rx_buffer, "STATE=7") == 0)
            {
                 led7();
            }
            if (strcmp(rx_buffer, "STATE=8") == 0)
            {
                 led8();
            }

            int to_write = len;
            while (to_write > 0)
            {
                int written = send(sock, rx_buffer + (len - to_write), to_write, 0); // vypis dorucenych dat od klienta
                if (written < 0)
                {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    return;
                }
                to_write -= written;
            }
        }
    } while (len > 0);
}

static void tcp_server_task(void *pvParameters)
{ // task servera, prirazduje config wifi a pocuva na sockete kto sa pripoji
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol); // err - nevytvoril sa socket na komunikaciu
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT); // vypise info o naviazanom sockete

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            continue;
        }

        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG, "Socket accepted IP address: %s", addr_str);

        do_retransmit(sock);

        shutdown(sock, 0); // uzatvorenie socketu
        close(sock);
    }

CLEAN_UP:
    close(listen_sock); // vycistenie socketu
    vTaskDelete(NULL);
}

void app_main(void)
{
    
    ESP_ERROR_CHECK(nvs_flash_init()); // tieto 3 su funkcie na init "internetu" ipciek a pod
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect()); // toto je funkcia zodpovedna za to, ze taha nastavenie siete, hesla, portu a pod z menuconfigu a nemusis to pisat do kodu
    
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL);
}