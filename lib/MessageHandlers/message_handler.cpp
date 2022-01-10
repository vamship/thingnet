#include <Arduino.h>

#include "messages.h"
#include "message_handler.h"
#include "log.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("msgh");

namespace thingnet::message_handlers
{
    MessageHandler::MessageHandler()
    {
    }

    MessageHandler::~MessageHandler()
    {
        // Nothing to release here.
    }

    bool MessageHandler::can_handle(PeerMessage *message)
    {
        LOG_DEBUG_1(logger, "[MessageHandler] Checking if handler can process message");
        return true;
    }

    ProcessingResult MessageHandler::process(PeerMessage *message)
    {
        LOG_DEBUG_1(logger, "[MessageHandler] Processing message");
        return ProcessingResult::handled;
    }
}