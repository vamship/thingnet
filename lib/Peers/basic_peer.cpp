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

        int result = RESULT_OK;
        if (message->payload.type == MSG_TYPE_HEARTBEAT)
        {
            LOG_DEBUG("Acknowledging heartbeat from [%s]",
                      LOG_FORMAT_MAC(message->sender));

            MessagePayload payload(MSG_TYPE_ACK);
            memcpy(payload.body, &message->payload.message_id, 2);

            result = this->node->send_message((u8 *)message->sender,
                                              &payload, 2);
        }
        else if (message->payload.type == MSG_TYPE_ACK)
        {
            LOG_DEBUG("Ack received from [%s]",
                      LOG_FORMAT_MAC(message->sender));
        }
        else
        {
            LOG_WARN("Unexpected message [%02x] from [%s]",
                     message->payload.type,
                     LOG_FORMAT_MAC(message->sender));
        }

        return result == RESULT_OK ? ProcessingResult::handled : ProcessingResult::error;
    }

    bool BasicPeer::is_active()
    {
        return (millis() - this->last_message_time) < this->timeout;
    }

    int BasicPeer::update()
    {
        LOG_INFO("Sending heartbeat message to peer");
        MessagePayload payload(MSG_TYPE_HEARTBEAT);

        this->node->send_message((u8 *)this->peer_mac_address, &payload, 6);

        return RESULT_OK;
    }

}