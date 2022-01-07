#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "client_node_manager.h"

namespace thingnet::node_managers
{
    const int __CLIENT_NODE_MESSAGE_DURATION = 30000;
    const u8 __BROADCAST_PEER[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ClientNodeManager::ClientNodeManager(EspNowNode *node) : NodeManager(node)
    {
        this->message_timer = new Timer(__CLIENT_NODE_MESSAGE_DURATION, true);
    }

    int ClientNodeManager::init()
    {
        ASSERT_OK(NodeManager::init());

        LOG_INFO("Starting message timer");
        this->message_timer->start();

        LOG_INFO("Client node manager initialized");
        return RESULT_OK;
    }

    int ClientNodeManager::update()
    {
        ASSERT_OK(NodeManager::update());

        if (this->message_timer->is_complete())
        {
            LOG_INFO("Sending message to server");
            for (u8 index = 0; index < this->peer_count; index++)
            {
                LOG_DEBUG("Sending message to peer [%d]", index);
                this->peer_list[index]->update();
            }
        }

        return RESULT_OK;
    }

    Peer *ClientNodeManager::create_peer(PeerMessage *message)
    {
        LOG_INFO("Inspecting message from peer");
        if (message->payload.message_id != MSG_TYPE_ADVERTISEMENT)
        {
            LOG_INFO("Unexpected message [%d] from [%s]",
                     message->payload.message_id,
                     LOG_FORMAT_MAC(message->sender));

            return 0;
        }
        LOG_INFO("Advertisement message received from [%s]",
                 LOG_FORMAT_MAC(message->sender));
        return 0;
    }
}