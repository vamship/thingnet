#include <Arduino.h>

#include "log.h"
#include "error_codes.h"
#include "messages.h"
#include "basic_peer.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("basic-peer");

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
        LOG_TRACE(logger, "Processing message");
        this->last_message_time = millis();

        int result = RESULT_OK;
        if (message->payload.type == MSG_TYPE_HEARTBEAT)
        {
            LOG_DEBUG(logger, "[HEARTBEAT] Message id [%d] from [%s]",
                      message->payload.message_id,
                      LOG_FORMAT_MAC(message->sender));

            MessagePayload payload(MSG_TYPE_ACK);
            memcpy(payload.body, &message->payload.message_id, 2);

            result = this->node->send_message((u8 *)message->sender,
                                              &payload, 2);
        }
        else if (message->payload.type == MSG_TYPE_ACK)
        {
            LOG_DEBUG(logger, "[ACK] from [%s] for message id [%02x:%02x]",
                      LOG_FORMAT_MAC(message->sender),
                      message->payload.body[0],
                      message->payload.body[1]);
        }
        else if (message->payload.type == MSG_TYPE_ADVERTISEMENT)
        {
            u8 mac_addr[6];
            memcpy(mac_addr, message->payload.body, 6);
            LOG_DEBUG(logger, "[ADVERTISEMENT] from [%s] for [%s]",
                     LOG_FORMAT_MAC(message->sender),
                     LOG_FORMAT_MAC(mac_addr));
        }
        else
        {
            LOG_WARN(logger, "[UNKNOWN] Message type [%02x] from [%s]",
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
        LOG_DEBUG(logger, "Sending heartbeat message to peer");
        MessagePayload payload(MSG_TYPE_HEARTBEAT);

        this->node->send_message((u8 *)this->peer_mac_address, &payload, 6);

        return RESULT_OK;
    }

}