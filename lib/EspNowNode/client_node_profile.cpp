#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "client_node_profile.h"
#include "basic_peer.h"

using namespace thingnet::peers;

namespace thingnet
{
    const int __CLIENT_NODE_PROFILE_DEFAULT_UPDATE_PERIOD = 10000;

    ClientNodeProfile::ClientNodeProfile(EspNowNode *node) : NodeProfile(node)
    {
        this->message_timer = new Timer(
            __CLIENT_NODE_PROFILE_DEFAULT_UPDATE_PERIOD,
            true);
    }

    int ClientNodeProfile::init()
    {
        ASSERT_OK(NodeProfile::init());

        LOG_INFO("Starting message timer");
        this->message_timer->start();

        LOG_INFO("Peer node manager initialized");
        return RESULT_OK;
    }

    int ClientNodeProfile::update()
    {
        ASSERT_OK(NodeProfile::update());

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

    Peer *ClientNodeProfile::create_peer(PeerMessage *message)
    {
        LOG_INFO("Processing create peer request");

        if (message->payload.type != MSG_TYPE_ADVERTISEMENT)
        {
            LOG_DEBUG("Unexpected message [%02x] from [%s]. Ignoring.",
                      message->payload.type,
                      LOG_FORMAT_MAC(message->sender));

            return 0;
        }
        u8 mac_addr[6];
        memcpy(mac_addr, message->payload.body, 6);
        LOG_INFO("Advertisement message received for [%s] from [%s]",
                 LOG_FORMAT_MAC(message->sender),
                 LOG_FORMAT_MAC(mac_addr));

        return new BasicPeer(this->node, mac_addr);
    }
}