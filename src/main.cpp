#include <Arduino.h>
#ifdef ESP32
  #include<Wifi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"
#include "format_utils.h"
#include "esp_now_node.h"
#include "peer_message_handler.h"

const u8 ALL_PEERS_LEN = 2;
u8 ALL_PEERS[][6] = {
  {0x18, 0xFE, 0x34, 0xD3, 0xF1, 0x21}, // White
  {0x18, 0xFE, 0x34, 0xD4, 0x82, 0x2A} // Blue
};

typedef struct MessageData {
  char text[32];
} MessageData;
MessageData message;

EspNowNode& node = EspNowNode::get_instance();

u64 last_time = 0;
u64 send_interval = 3000;
u8 node_mac_address[6];
u8 *peers[ALL_PEERS_LEN - 1];

void setup() {

  Serial.begin(115200);

  LOG_DEBUG("Initializing GPIO pins");
  pinMode(LED_BUILTIN, OUTPUT);

  if (node.init() != 0) {
    exit(1);
  }
  WiFi.macAddress(node_mac_address);

  LOG_DEBUG("Initializing default message processor");
  node.set_default_handler(new MessageHandler());

  LOG_DEBUG("Adding peers");
  u8 peer_index = 0;
  for (u8 all_peers_index = 0; all_peers_index < ALL_PEERS_LEN; all_peers_index++) {

    bool is_self_address = true;
    for (u8 byte_index=0; byte_index < 6; byte_index++) {
      if (ALL_PEERS[all_peers_index][byte_index] != node_mac_address[byte_index]) {
        is_self_address = false;
      }
    }

    if (!is_self_address) {
      if (peer_index >= ALL_PEERS_LEN - 1) {
        LOG_ERROR("Found more than expected peers");
        exit(1);
      }

      PeerMessageHandler *handler = new PeerMessageHandler(ALL_PEERS[all_peers_index]);
      node.add_handler((MessageHandler *)handler);

      peers[peer_index] = ALL_PEERS[all_peers_index];
      esp_now_add_peer(peers[peer_index], ESP_NOW_ROLE_COMBO, 1, NULL, 0);

      peer_index++;
    }
  }

  LOG_INFO("Initialization complete");
}

void loop() {
  if ((millis() - last_time) > send_interval) {
    last_time = millis();

    digitalWrite(LED_BUILTIN, HIGH);

    LOG_DEBUG("Preparing message");
    strcpy(message.text, "Hello");

    for (u8 peer_index = 0; peer_index < ALL_PEERS_LEN - 1; peer_index++) {
      LOG_DEBUG("Sending message to peer");
      esp_now_send(peers[peer_index], (u8 *) &message, sizeof(message));

      LOG_DEBUG("Message sent");
    }

    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
}
