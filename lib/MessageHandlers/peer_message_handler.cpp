#include "peer_message_handler.h"

#include <Arduino.h>

#include "message_handler.h"
#include "log.h"

namespace thingnet::message_handlers
{
    PeerMessageHandler::PeerMessageHandler(u8 *peer_mac_address)
    {
        // Store the peer mac address for future comparison
        memcpy(this->peer_mac_address, peer_mac_address, 6);
    }

    bool PeerMessageHandler::can_handle(PeerMessage *message)
    {
        if (memcmp(this->peer_mac_address, message->sender, 6) == 0)
        {
            LOG_DEBUG("[PeerMessageHandler] will handle message");
            return true;
        };

        LOG_DEBUG("[PeerMessageHandler] will not handle message");
        return false;
    }

    ProcessingResult PeerMessageHandler::process(PeerMessage *message)
    {
        LOG_DEBUG("[PeerMessageHandler] Processing message");
        return ProcessingResult::handled;
    }
}