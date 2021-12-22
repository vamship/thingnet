#ifdef ESP32
  #include<Wifi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "esp_now_node.h"
#include "peer_message.h"
#include "log.h"
#include "format_utils.h"

MessageHandler message_processor_list[10];
int message_processor_count = 0;

/**
 * @brief Handles data send confirmation.
 * 
 * @param mac_addr The mac address of the recipient
 * @param status Whether or not the message was sent successfully
 */
void __on_data_sent(u8 *mac_addr, u8 status) {
  char buffer[18];
  FORMAT_MAC(buffer, mac_addr);
  LOG_INFO("Message delivery to [%s] [%s]", buffer, status==0?"ok":"err");
}

/**
 * @brief Handles data received from peers
 * 
 * @param mac_addr The mac address of the sender
 * @param data The payload received from the sender
 * @param length The length of the payload
 */
void __on_data_received(u8 *mac_addr, u8 *data, u8 length) {
  LOG_INFO("Message received from peer");

  LOG_DEBUG("Preparing message payload");
  PeerMessage message;
  memcpy(&message, mac_addr, 6);
  memcpy(&message + 6, data, length);

  LOG_DEBUG("Processing message");
  for(int index = message_processor_count - 1; index >= 0; index--) {
    MessageHandler processor = message_processor_list[index];
    if(processor.can_handle(&message)) {
      LOG_DEBUG("Processor [%d] can handle message", index);
      ProcessingResult result = processor.process(&message);

      if(result == ProcessingResult::handled) {
        LOG_INFO("Processing chain completed");
        break;
      } else if (result == ProcessingResult::chain) {
        continue;
      } else {
        LOG_ERROR("Error processing message");
        break;
      }
    }
  }
}

EspNowNode::EspNowNode(){ }

EspNowNode& EspNowNode::get_instance() {
    static EspNowNode instance;
    return instance;
}

int EspNowNode::init() {
  if (this->is_initialized) {
    LOG_INFO("Node has already been initialized");
    return 0;
  }

  LOG_DEBUG("Setting wifi to station mode");
  WiFi.mode(WIFI_AP_STA);

  LOG_DEBUG("Reading current mac address");
  this->mac_address = WiFi.macAddress();

  LOG_DEBUG("Initializing ESP-NOW")
  if (esp_now_init() != 0) {
    LOG_ERROR("Error initializing ESP-NOW");
    return 1;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  LOG_DEBUG("Initializing default message processor");
  message_processor_list[message_processor_count] = MessageHandler();
  message_processor_count++;

  LOG_DEBUG("Registering send/receive callbacks");
  esp_now_register_send_cb(__on_data_sent);
  esp_now_register_recv_cb(__on_data_received);


  return 0;
}
