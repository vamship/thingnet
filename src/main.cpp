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
#include "node.h"
#include "node_profile.h"
#include "server_node_profile.h"
#include "client_node_profile.h"

#include "hadrware_manager.h"

using namespace thingnet;
using namespace thingnet::message_handlers;
using namespace thingnet::peers;
using namespace thingnet::utils;

static Logger *logger = new Logger("main");
static Node &node = Node::get_instance();

static Timer *status_timer = new Timer(10000, true);
static HardwareManager *hw_manager = new HardwareManager();

void setup()
{
    Serial.begin(115200);

    LOG_DEBUG(logger, "Determining node profile");
    NodeProfile *profile;

    LOG_DEBUG(logger, "Initializing hardware manager");
    hw_manager->initialize();

    LOG_DEBUG(logger, "Performing preliminary update on hardware manager");
    ASSERT_OK(hw_manager->update());

    if (hw_manager->is_server_mode())
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
    ASSERT_OK(node.update());
    ASSERT_OK(hw_manager->update());

    if (hw_manager->send_advertisement())
    {
        LOG_INFO(logger, "Advertising server");
        ServerNodeProfile *profile = (ServerNodeProfile *)node.get_profile();
        ASSERT_OK(profile->advertise());
    }

    if (status_timer->is_complete())
    {
        LOG_INFO(logger, "-==-");
        LOG_INFO(logger, "Profile    : [%s]",
                 hw_manager->is_server_mode() ? "SERVER" : "CLIENT");
        LOG_INFO(logger, "Peer count : [%d]", node.get_profile()->get_peer_count());
        LOG_INFO(logger, "-==-");
    }
}