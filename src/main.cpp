#include <Arduino.h>
#ifdef ESP32
  #include<Wifi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#define LOG_ENABLED
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "log.h"
#include "format_utils.h"

// #define __CONTROLLER_MODE

const u8 ALL_PEERS_LEN = 2;
u8 ALL_PEERS[][6] = {
  {0x18, 0xFE, 0x34, 0xD3, 0xF1, 0x21}, // White
  {0x18, 0xFE, 0x34, 0xD4, 0x82, 0x2A} // Blue
};

u64 last_time = 0;
u64 send_interval = 3000;
u8 node_mac_address[6];
u8 *peers[ALL_PEERS_LEN - 1];

// Message data structure
typedef struct message {
  char text[32];
} message;

// Variable to hold data that is being sent
message messageData;

/**
 * @brief Handles data send confirmation.
 * 
 * @param mac_addr The mac address of the recipient
 * @param status Whether or not the message was sent successfully
 */
void on_data_sent(u8 *mac_addr, u8 status) {
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
void on_data_received(u8 *mac_addr, u8 *data, u8 length) {
  LOG_INFO("Message received");
  memcpy(&messageData, data, length);
  LOG_DEBUG(messageData.text);
}

void setup() {

  Serial.begin(115200);

  LOG_DEBUG("Initializing GPIO pins");
  pinMode(LED_BUILTIN, OUTPUT);

  LOG_DEBUG("Setting wifi to station mode");
  WiFi.mode(WIFI_AP_STA);

  LOG_DEBUG("Reading current mac address");
  WiFi.macAddress(node_mac_address);

  LOG_DEBUG("Initializing ESP-NOW")
  if (esp_now_init() != 0) {
    LOG_ERROR("Error initializing ESP-NOW");
    exit(1);
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  LOG_DEBUG("Registering send/receive callbacks");
  esp_now_register_send_cb(on_data_sent);
  esp_now_register_recv_cb(on_data_received);

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
      peers[peer_index] = ALL_PEERS[all_peers_index];
      esp_now_add_peer(peers[peer_index], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

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
    strcpy(messageData.text, "Hello");

    for (u8 peer_index = 0; peer_index < ALL_PEERS_LEN - 1; peer_index++) {
      LOG_DEBUG("Sending message to peer");
      esp_now_send(peers[peer_index], (u8 *) &messageData, sizeof(messageData));

      LOG_DEBUG("Message sent");
    }

    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
}