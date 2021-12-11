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

// #define __CONTROLLER_MODE

u8 PEER_ADDRESSES[][6] = {
  {0x18, 0xFE, 0x34, 0xD3, 0xF1, 0x21}, // White
  {0x18, 0xFE, 0x34, 0xD4, 0x82, 0x2A} // Blue
};
u8 PEER_ADDRESS_LEN = 2;

u64 last_time = 0;
u64 send_interval = 3000;

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
  LOG_INFO(status == 0 ? "Message delivery succeeded" : "Message delivery failed");
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
  for (u8 peer_index = 0; peer_index < PEER_ADDRESS_LEN; peer_index++) {
    esp_now_add_peer(PEER_ADDRESSES[peer_index], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }

  LOG_INFO("Initialization complete");
}

void loop() {
  if ((millis() - last_time) > send_interval) {
    last_time = millis();

    digitalWrite(LED_BUILTIN, HIGH);

    LOG_DEBUG("Preparing message");
    strcpy(messageData.text, "Hello");

    for (u8 peer_index = 0; peer_index < PEER_ADDRESS_LEN; peer_index++) {
      LOG_DEBUG("Sending message to peer");
      esp_now_send(PEER_ADDRESSES[peer_index], (u8 *) &messageData, sizeof(messageData));

      LOG_DEBUG("Message sent");
    }

    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
}