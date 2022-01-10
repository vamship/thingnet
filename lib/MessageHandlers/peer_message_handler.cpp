#include "peer_message_handler.h"

#include <Arduino.h>

#include "messages.h"
#include "message_handler.h"
#include "log.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("peer-msgh");

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
            LOG_DEBUG_1(logger, "[PeerMessageHandler] will handle message");
            return true;
        };

        LOG_DEBUG_1(logger, "[PeerMessageHandler] will not handle message");
        return false;
    }

    ProcessingResult PeerMessageHandler::process(PeerMessage *message)
    {
        LOG_DEBUG_1(logger, "[PeerMessageHandler] Processing message");
        return ProcessingResult::handled;
    }
}