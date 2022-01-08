#include <Arduino.h>

#include "log.h"
#include "error_codes.h"
#include "messages.h"
#include "basic_peer.h"

namespace thingnet::peers
{
    BasicPeer::BasicPeer(EspNowNode *node, u8 *peer_mac_address, u32 timeout)
        : Peer(node, peer_mac_address)
    {
        this->timeout = timeout;
        this->last_message_time = millis();
    }

    BasicPeer::BasicPeer(EspNowNode *node, u8 *peer_mac_address)
        : BasicPeer(node, peer_mac_address, BASIC_PEER_DEFAULT_TIMEOUT)
    {
    }

    ProcessingResult BasicPeer::process(PeerMessage *message)
    {
        LOG_DEBUG("[BasicPeer] Processing message");
        this->last_message_time = millis();

        if (message->payload.type == MSG_TYPE_HEARTBEAT)
        {
            LOG_DEBUG("Received heartbeat from [%s]",
                      LOG_FORMAT_MAC(message->sender));
        }
        return ProcessingResult::handled;
    }

    bool BasicPeer::is_active()
    {
        return (millis() - this->last_message_time) < this->timeout;
    }

    int BasicPeer::update()
    {
        LOG_INFO("Sending heartbeat message to peer");
        MessagePayload payload;
        payload.type = MSG_TYPE_HEARTBEAT;

        this->node->send_message((u8 *)this->peer_mac_address, &payload, 6);

        return RESULT_OK;
    }

}