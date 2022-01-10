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
#include "client_node_profile.h"

using namespace thingnet;
using namespace thingnet::message_handlers;
using namespace thingnet::peers;
using namespace thingnet::utils;

// const u8 SERVER_MAC[] = {0x18, 0xfe, 0x34, 0xd4, 0x7e, 0x9a};
static const u8 SERVER_MAC[] = {0x1A, 0xFE, 0x34, 0xD4, 0x82, 0x2A};
static EspNowNode &node = EspNowNode::get_instance();
static Logger *logger = new Logger("main");

static Timer *status_timer = new Timer(10000, true);
static bool is_server_profile = false;

bool is_server()
{
    u8 ap_mac_addr[7];
    u8 sta_mac_addr[6];

    WiFi.macAddress(sta_mac_addr);
    WiFi.softAPmacAddress(ap_mac_addr);

    return memcmp(sta_mac_addr, SERVER_MAC, 6) == 0 ||
           memcmp(ap_mac_addr, SERVER_MAC, 6) == 0;
}

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG(logger, "Determining node profile");
    NodeProfile *profile;

    is_server_profile = is_server();
    if (is_server_profile)
    {
        profile = new ServerNodeProfile(&node);
        LOG_INFO(logger, "Node profile is [SERVER]");
    }
    else
    {
        profile = new ClientNodeProfile(&node);
        LOG_INFO(logger, "Node profile is [CLIENT]");
    }

    node.set_node_profile(profile);

    LOG_DEBUG(logger, "Initializing node");
    ASSERT_OK(node.init());

    LOG_DEBUG(logger, "Starting status timer");
    status_timer->start();

    LOG_INFO(logger, "Initialization complete");
}

void loop()
{
    node.update();

    if (status_timer->is_complete())
    {
        LOG_INFO(logger, "-==-");
        LOG_INFO(logger, "Profile    : [%s]", is_server_profile ? "SERVER" : "CLIENT");
        LOG_INFO(logger, "Peer count : [%d]", node.get_profile()->get_peer_count());
        LOG_INFO(logger, "-==-");
    }
}