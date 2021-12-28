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
#include "node_manager.h"
#include "server_node_manager.h"

#include "message_handler.h"
#include "peer_message_handler.h"
#include "generic_message_handler.h"

#define UPDATE_INTERVAL 6000
#define UPDATE_NOTIFICATION_INTERVAL 2000
#define PULSE_LONG_DURATION 1000
#define PULSE_SHORT_DURATION 500

// const u8 SERVER_MAC[] = {0x18, 0xfe, 0x34, 0xd4, 0x7e, 0x9a};
const u8 SERVER_MAC[] = {0x1A, 0xFE, 0x34, 0xD4, 0x82, 0x2A};

EspNowNode &node = EspNowNode::get_instance();
NodeManager *manager = new ServerNodeManager(&node);

Pulser *led;
Timer *update_timer;
Timer *led_reset_timer;

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG("Initializing node")
    ASSERT_OK(node.init());

    LOG_DEBUG("Determining run mode");
    if (node.has_mac_address((u8 *)SERVER_MAC))
    {
        ASSERT_OK(node.set_default_handler(manager));
        LOG_INFO("Running as SERVER");
    }
    else
    {
        /// TODO: Need to implement this
        LOG_INFO("Running as PEER")
    }

    LOG_DEBUG("Initializing LED");
    led = new Pulser(LED_BUILTIN, PULSE_LONG_DURATION);
    led->init();

    LOG_DEBUG("Initialize timers");
    update_timer = new Timer(UPDATE_INTERVAL);
    led_reset_timer = new Timer(UPDATE_NOTIFICATION_INTERVAL);

    update_timer->start();

    LOG_INFO("Initialization complete");
}

void loop()
{
    if (update_timer->is_complete())
    {
        LOG_DEBUG("Running manager update");
        manager->update();

        led->set_duration(PULSE_SHORT_DURATION);
        led_reset_timer->start();
    }

    if (led_reset_timer->is_complete())
    {
        led->set_duration(PULSE_LONG_DURATION);
        update_timer->start();
    }

    led->update();
}