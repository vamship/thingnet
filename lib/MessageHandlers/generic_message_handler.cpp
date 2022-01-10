#include <Arduino.h>

#include "log.h"
#include "messages.h"
#include "message_handler.h"
#include "generic_message_handler.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("gen-msgh");

namespace thingnet::message_handlers
{
    GenericMessageHandler::GenericMessageHandler(message_processor_t processor)
    {
        this->processor = processor;
    }

    ProcessingResult GenericMessageHandler::process(PeerMessage *message)
    {
        LOG_DEBUG(logger, "[GenericMessageHandler] Delegating message handling");
        return this->processor(message);
    }
}