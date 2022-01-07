#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "peer_node_manager.h"

namespace thingnet::node_managers
{
    const int __PEER_NODE_MESSAGE_DURATION = 30000;

    PeerNodeManager::PeerNodeManager(EspNowNode *node) : NodeManager(node)
    {
        this->message_timer = new Timer(__PEER_NODE_MESSAGE_DURATION, true);
    }

    int PeerNodeManager::init()
    {
        ASSERT_OK(NodeManager::init());

        LOG_INFO("Starting message timer");
        this->message_timer->start();

        LOG_INFO("Peer node manager initialized");
        return RESULT_OK;
    }

    int PeerNodeManager::update()
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

    Peer *PeerNodeManager::create_peer(PeerMessage *message)
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