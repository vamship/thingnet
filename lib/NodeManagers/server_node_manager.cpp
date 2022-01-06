#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "basic_peer.h"
#include "server_node_manager.h"

namespace thingnet::node_managers
{
    u16 message_id = 0;
    const int __SERVER_NODE_ADVERTISE_DURATION = 30000;
    const u8 __BROADCAST_PEER[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ServerNodeManager::ServerNodeManager(EspNowNode *node) : NodeManager(node)
    {
        this->advertise_timer = new Timer(__SERVER_NODE_ADVERTISE_DURATION,
                                          true);
    }

    int ServerNodeManager::init()
    {
        ASSERT_OK(NodeManager::init());

        LOG_INFO("Starting advertise timer");
        this->advertise_timer->start();

        ASSERT_OK(register_peer((u8 *)__BROADCAST_PEER, ESP_NOW_ROLE_CONTROLLER));

        LOG_INFO("Server node manager initialized");
        return RESULT_OK;
    }

    int ServerNodeManager::update()
    {
        ASSERT_OK(NodeManager::update());

        if (this->advertise_timer->is_complete())
        {
            LOG_INFO("Advertising server to peers");
            MessagePayload payload;
            payload.type = MSG_TYPE_ADVERTISEMENT;
            payload.message_id = ++message_id;
            this->node->read_mac_address(payload.body);

            send_message((u8 *)__BROADCAST_PEER, &payload, 9);
        }

        return RESULT_OK;
    }
}