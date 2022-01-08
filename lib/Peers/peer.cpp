#include <Arduino.h>

#include "log.h"
#include "error_codes.h"

#include "esp_now_node.h"
#include "messages.h"
#include "peer.h"

namespace thingnet::peers
{
    Peer::Peer(EspNowNode *node, u8 *peer_mac_address)
    {
        this->node = node;
        memcpy(this->peer_mac_address, peer_mac_address, 6);
    }

    int Peer::read_mac_address(u8 *buffer)
    {
        memcpy(buffer, this->peer_mac_address, 6);
        return RESULT_OK;
    }

    bool Peer::can_handle(PeerMessage *message)
    {
        if (memcmp(this->peer_mac_address, message->sender, 6) == 0)
        {
            LOG_DEBUG("[Peer] will handle message");
            return true;
        };

        LOG_DEBUG("[Peer] will not handle message");
        return false;
    }

    Peer::~Peer()
    {
        // Nothing to do here
    }

    int Peer::update()
    {
        return RESULT_OK;
    }
}