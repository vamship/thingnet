#include <Arduino.h>
#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"
#include "format_utils.h"
#include "pulser.h"
#include "timer.h"
#include "error_codes.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "peer_message_handler.h"
#include "generic_message_handler.h"

#define SEND_INTERVAL 6000
#define SEND_INDICATION_INTERVAL 2000
#define PULSE_LONG_DURATION 1000
#define PULSE_SHORT_DURATION 500

const u8 SERVER_MAC[] = {0x18, 0xfe, 0x34, 0xd4, 0x7e, 0x9a};

EspNowNode &node = EspNowNode::get_instance();

Pulser *led;
Timer *send_timer;
Timer *led_reset_timer;
u8 peer_list[255][6];
u8 peer_count = 0;

ProcessingResult __add_new_peer(PeerMessage *message)
{
    LOG_DEBUG("Checking if peer has previously been registered");
    for (int index = 0; index < peer_count; index++)
    {
        if (memcmp(peer_list[index], message->sender, 6) == 0)
        {
            LOG_INFO("Peer already registered");
            return ProcessingResult::handled;
        }
    }

    LOG_DEBUG("Adding peer to list");
    memcpy(peer_list[peer_count], message->sender, 6);
    peer_count++;

    LOG_DEBUG("Configuring peer");
    esp_now_add_peer(message->sender, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    LOG_DEBUG("Setting up message handler for peer");
    node.add_handler(new PeerMessageHandler(message->sender));

    LOG_INFO("New peer registered");
    return ProcessingResult::handled;
}

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG("Initializing node")
    ASSERT_OK(node.init());

    LOG_DEBUG("Initializing default message processor");
    ASSERT_OK(node.set_default_handler(new GenericMessageHandler(__add_new_peer)));

    if (node.has_mac_address((u8 *)SERVER_MAC))
    {
        LOG_INFO("Running as SERVER");
    }
    else
    {
        LOG_INFO("Running as PEER")
        memcpy(peer_list[peer_count], SERVER_MAC, 6);
        peer_count++;
    }

    LOG_DEBUG("Initializing LED");
    led = new Pulser(LED_BUILTIN, PULSE_LONG_DURATION);
    led->init();

    LOG_DEBUG("Initialize timers");
    send_timer = new Timer(SEND_INTERVAL);
    led_reset_timer = new Timer(SEND_INDICATION_INTERVAL);

    send_timer->start();

    LOG_INFO("Initialization complete");
}

void loop()
{
    if (send_timer->is_complete())
    {
        u8 payload[250];
        if (peer_count > 0)
        {
            LOG_DEBUG("Preparing message");
            payload[0] = 5; // Length of the message (Hello)
            strcpy((char *)payload + 1, "Hello");
        }
        else
        {
            LOG_DEBUG("No peers registered");
        }

        for (u8 peer_index = 0; peer_index < peer_count; peer_index++)
        {
            LOG_DEBUG("Sending message to peer");
            esp_now_send(peer_list[peer_index], (u8 *)&payload, sizeof(payload));

            LOG_DEBUG("Message sent");
        }

        led->set_duration(PULSE_SHORT_DURATION);
        led_reset_timer->start();
    }

    if (led_reset_timer->is_complete())
    {
        led->set_duration(PULSE_LONG_DURATION);
        send_timer->start();
    }

    led->update();
}