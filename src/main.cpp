#include <Arduino.h>
#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"
#include "pulser.h"
#include "timer.h"
#include "error_codes.h"
#include "esp_now_node.h"
#include "server_node_manager.h"

using namespace thingnet;
using namespace thingnet::node_managers;
using namespace thingnet::message_handlers;

// const u8 SERVER_MAC[] = {0x18, 0xfe, 0x34, 0xd4, 0x7e, 0x9a};
const u8 SERVER_MAC[] = {0x1A, 0xFE, 0x34, 0xD4, 0x82, 0x2A};

EspNowNode &node = EspNowNode::get_instance();

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG("Initializing node")
    ASSERT_OK(node.init());

    LOG_DEBUG("Determining run mode");

    NodeManager *manager;

    if (node.has_mac_address((u8 *)SERVER_MAC))
    {
        manager = new ServerNodeManager(&node);
        LOG_INFO("Running in SERVER mode");
    }
    else
    {
        /// TODO: Need to implement this
        manager = new ServerNodeManager(&node);
        LOG_INFO("Running in CLIENT mode");
    }

    ASSERT_OK(manager->init());
    ASSERT_OK(node.set_node_manager(manager));

    LOG_INFO("Initialization complete");
}

void loop()
{
    node.update();
}