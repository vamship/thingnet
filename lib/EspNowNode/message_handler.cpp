#include <Arduino.h>

#include "message_handler.h"
#include "log.h"
#include "format_utils.h"

MessageHandler::MessageHandler() {}

bool MessageHandler::can_handle(PeerMessage *message)
{
    LOG_DEBUG("[MessageHandler] Checking if handler can process");
    return true;
}

ProcessingResult MessageHandler::process(PeerMessage *message)
{
    LOG_DEBUG("[MessageHandler] Processing message");
    return ProcessingResult::handled;
}