#include <Arduino.h>

#include "log.h"
#include "basic_peer.h"

namespace thingnet::peers
{
    BasicPeer::BasicPeer(u8 *peer_mac_address, u32 timeout)
    {
        // Store the peer mac address for future comparison
        memcpy(this->peer_mac_address, peer_mac_address, 6);
        this->timeout = timeout;
        this->last_message_time = millis();
    }

    BasicPeer::BasicPeer(u8 *peer_mac_address)
        : BasicPeer(peer_mac_address, 30000)
    {
    }

    bool BasicPeer::can_handle(PeerMessage *message)
    {
        if (memcmp(this->peer_mac_address, message->sender, 6) == 0)
        {
            LOG_DEBUG("[BasicPeer] will handle message");
            return true;
        };

        LOG_DEBUG("[BasicPeer] will not handle message");
        return false;
    }

    ProcessingResult BasicPeer::process(PeerMessage *message)
    {
        LOG_DEBUG("[BasicPeer] Processing message");
        this->last_message_time = millis();
        return ProcessingResult::handled;
    }

    bool BasicPeer::is_active()
    {
        return (millis() - this->last_message_time) < this->timeout;
    }

}