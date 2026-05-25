#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "WiFi Sniffer";// should be called "Listener" lol
// create structure here for all the key variables
typedef struct {
    int rssi;// received signal strength indicator
    uint32_t length;// length of packet
    uint8_t transmitter_mac[6];// mac address
} sniffer_packet_t;

static QueueHandle_t packet_queue = NULL; // initialize queue to zero

// function to accept the packets. ONE PACKET AT A TIME.
void wifi_packet_handler(void* buf,wifi_promiscuous_pkt_type_t type)
{
    if (buf == NULL) //null pointer guard. Prevents it for searching for memory address 
     //that doesnt exist.
    {
        return;
    }
    // zero copy typecasting. Dont have time to copy the data. new var called pkt
    // * before pkt indicates it is a pointer
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    // store pkt received signal strength
    int rssi = pkt->rx_ctrl.rssi;
    // store pkt length
    uint32_t pkt_length = pkt->rx_ctrl.sig_len;
    // pointer to the start of the wireless frame
    // 8 bits = 1 byte. we want to read the data byte by byte
    uint8_t *raw_payload = pkt->payload;
    // dont print status here since it is extremely time consuming.

    // Create a temporary envelope
    sniffer_packet_t message;

    // add to structure envelope
    message.rssi = rssi;
    message.length = pkt_length;
    memcpy(message.transmitter_mac, &raw_payload[16], 6);
    // copy memory mac address from one location of RAM to another
    // destination: message.transmitter_mac
    //source: &raw_payload[10]
    //according to the international 802.11 standard, the sender's MAC address always begins exactly at the 10th byte of the frame header
    //6: size in bytes

    // Drop the envelope into the mailbox
    //line to initialize higher priority task token
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // send envelope to rtos regardless of priority 
    xQueueSendToBackFromISR(packet_queue, &message, &xHigherPriorityTaskWoken);
    // the queue function updates the priority to true
   // if no higher priority task exists, go to while(1) immediately upon exiting this function
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }

}

// pvParameters is a void pointer just for initialization
void rtos_task(void *pvParameters)
{
ESP_LOGI("RTOS CORE","rtos task has started running");
// to store the data from the envelope
sniffer_packet_t received_pkt;

while(1)
{
    if (xQueueReceive(packet_queue, &received_pkt, portMAX_DELAY) == pdTRUE)
    {
                printf("%lu,1,%lu,%d,%02X:%02X:%02X:%02X:%02X:%02X\n",
        (unsigned long)esp_log_timestamp(),
       (unsigned long)received_pkt.length,
       (int)received_pkt.rssi,
       received_pkt.transmitter_mac[0], 
       received_pkt.transmitter_mac[1], 
       received_pkt.transmitter_mac[2],
       received_pkt.transmitter_mac[3], 
       received_pkt.transmitter_mac[4], 
       received_pkt.transmitter_mac[5]);
    }
}
}

extern "C" void app_main(void)
{

    // 2. Initialize NVS (Crucial for Wi-Fi stack to start up!)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

  // create the queue of length 10 packets
  // first in first out buffer
  packet_queue = xQueueCreate(10,sizeof(sniffer_packet_t));
  // create rtos task titled packet printer task
  xTaskCreate(rtos_task,"Packet Printer Task",4096,NULL,5,NULL);
  // memory allocation: stack size is 4096 bytes
  // task parameters NULL
  // priority level 5 (1 being the lowest)
  //task handle NULL

  // now we need to initialize the wifi packet handler function to start it

  //1) Initialize network interface
  ESP_ERROR_CHECK(esp_netif_init());
  //2) Create an event loop, for hardware modules to indicate status
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  //3) configure wifi driver
  // cfg is an object of datatype wifi_init_config_t that contains instructions 
  // for initializing the wifi driver. we first create this object and pass 
  // to esp_wifi_init
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  //4) specifically configure the esp to store data in the ram not in permanent flash memory
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  //5) set wifi operating mode to 'STATION'
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  //6) START WIFI
  ESP_ERROR_CHECK(esp_wifi_start());
  //7) activate the sniffer-wifi packet handler
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_packet_handler));
  //8) listen to all packets
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  //9) select first channel
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
  // print status
  ESP_LOGI("SYSTEM", "Wi-Fi Sniffer is now fully armed and operational on Channel 1!");




}