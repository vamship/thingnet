#include <Arduino.h>
#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"
#include "format_utils.h"
#include "error_codes.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "peer_message_handler.h"
#include "generic_message_handler.h"

const u8 ALL_PEERS_LEN = 2;
u8 ALL_PEERS[][6] = {
    {0x18, 0xFE, 0x34, 0xD3, 0xF1, 0x21}, // White
    {0x18, 0xFE, 0x34, 0xD4, 0x82, 0x2A}  // Blue
};

typedef struct MessageData
{
    char text[32];
} MessageData;
MessageData message;

EspNowNode &node = EspNowNode::get_instance();

u64 last_time = 0;
u64 send_interval = 3000;
// u8 *peers[ALL_PEERS_LEN - 1];

ProcessingResult __add_new_peer(PeerMessage *message)
{
    LOG_INFO("Registering new peer");
    node.add_handler(new PeerMessageHandler(message->sender));
    esp_now_add_peer(message->sender, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    return ProcessingResult::handled;
}

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG("Initializing GPIO pins");
    pinMode(LED_BUILTIN, OUTPUT);

    LOG_DEBUG("Initializing node")
    ASSERT_OK(node.init());

    LOG_DEBUG("Initializing default message processor");
    ASSERT_OK(node.set_default_handler(new GenericMessageHandler(__add_new_peer)));

    LOG_INFO("Initialization complete");
}

void loop()
{
    if ((millis() - last_time) > send_interval)
    {
        last_time = millis();

        digitalWrite(LED_BUILTIN, HIGH);

        // LOG_DEBUG("Preparing message");
        // strcpy(message.text, "Hello");

        // for (u8 peer_index = 0; peer_index < ALL_PEERS_LEN - 1; peer_index++) {
        //   LOG_DEBUG("Sending message to peer");
        //   esp_now_send(peers[peer_index], (u8 *) &message, sizeof(message));

        //   LOG_DEBUG("Message sent");
        // }

        delay(1000);
    }
    digitalWrite(LED_BUILTIN, LOW);
}
