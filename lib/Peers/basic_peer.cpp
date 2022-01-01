#include <Arduino.h>

#include "log.h"
#include "messages.h"
#include "basic_peer.h"

namespace thingnet::peers
{
    BasicPeer::BasicPeer(u8 *peer_mac_address, u32 timeout): Peer(peer_mac_address)
    {
        this->timeout = timeout;
        this->last_message_time = millis();
    }

    BasicPeer::BasicPeer(u8 *peer_mac_address)
        : BasicPeer(peer_mac_address, 30000)
    {
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