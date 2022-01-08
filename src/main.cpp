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
#include "node_profile.h"
#include "server_node_profile.h"
#include "peer_node_manager.h"

using namespace thingnet;
using namespace thingnet::message_handlers;
using namespace thingnet::peers;
using namespace thingnet::utils;

// const u8 SERVER_MAC[] = {0x18, 0xfe, 0x34, 0xd4, 0x7e, 0x9a};
const u8 SERVER_MAC[] = {0x1A, 0xFE, 0x34, 0xD4, 0x82, 0x2A};

EspNowNode &node = EspNowNode::get_instance();

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG("Initializing node")
    ASSERT_OK(node.init());

    LOG_DEBUG("Determining run mode");

    NodeProfile *manager;

    if (node.has_mac_address((u8 *)SERVER_MAC))
    {
        manager = new ServerNodeProfile(&node);
        LOG_INFO("Running in SERVER mode");
    }
    else
    {
        manager = new PeerNodeManager(&node);
        LOG_INFO("Running in CLIENT mode");
    }

    ASSERT_OK(manager->init());
    ASSERT_OK(node.set_node_profile(manager));

    LOG_INFO("Initialization complete");
}

void loop()
{
    node.update();
}