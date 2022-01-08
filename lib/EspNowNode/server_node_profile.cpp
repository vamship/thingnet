#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "basic_peer.h"
#include "server_node_profile.h"

namespace thingnet
{
    const int __SERVER_NODE_PROFILE_DEFAULT_ADVERTISE_PERIOD = 60000;
    const u8 __BROADCAST_PEER[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ServerNodeProfile::ServerNodeProfile(EspNowNode *node) : NodeProfile(node)
    {
        this->advertise_period = __SERVER_NODE_PROFILE_DEFAULT_ADVERTISE_PERIOD;
        this->advertise_timer = 0;
    }

    int ServerNodeProfile::set_advertise_period(u32 timeout)
    {
        if (!this->is_initialized)
        {
            LOG_WARN("Node profile has not been initialized");
            return ERR_NODE_PROFILE_NOT_INITIALIZED;
        }

        this->advertise_period = timeout;

        return RESULT_OK;
    }

    int ServerNodeProfile::init()
    {
        ASSERT_OK(NodeProfile::init());

        LOG_INFO("Starting advertise timer");
        this->advertise_timer = new Timer(this->advertise_period, true);
        this->advertise_timer->start();

        ASSERT_OK(this->node->register_peer((u8 *)__BROADCAST_PEER,
                                            ESP_NOW_ROLE_CONTROLLER));

        LOG_INFO("Server node profile initialized");
        return RESULT_OK;
    }

    int ServerNodeProfile::update()
    {
        ASSERT_OK(NodeProfile::update());

        if (this->advertise_timer->is_complete())
        {
            LOG_INFO("Advertising server to peers");
            MessagePayload payload;
            payload.type = MSG_TYPE_ADVERTISEMENT;
            payload.message_id = this->node->get_next_message_id();
            this->node->read_mac_address(payload.body);

            this->node->send_message((u8 *)__BROADCAST_PEER, &payload, 6);
        }

        return RESULT_OK;
    }
}