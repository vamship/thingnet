#include "peer_message_handler.h"

#include <Arduino.h>

#include "message_handler.h"
#include "log.h"
#include "format_utils.h"

PeerMessageHandler::PeerMessageHandler(u8 peer_mac_address[6])
{
    // Store the peer mac address for future comparison
    memcpy(this->peer_mac_addres, peer_mac_addres, 6);
}

bool PeerMessageHandler::can_handle(PeerMessage *message)
{
    if (memcmp(this->peer_mac_addres, message->sender, 6))
    {
        LOG_INFO("[PeerMessageHandler] will handle message");
        return true;
    };
    LOG_INFO("[PeerMessageHandler] **CANNOT** handle message");
    return true;
}

ProcessingResult PeerMessageHandler::process(PeerMessage *message)
{
    LOG_INFO("[PeerMessageHandler] Processing message");
    return ProcessingResult::handled;
}