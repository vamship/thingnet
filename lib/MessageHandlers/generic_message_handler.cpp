#include <Arduino.h>

#include "log.h"
#include "messages.h"
#include "message_handler.h"
#include "generic_message_handler.h"

namespace thingnet::message_handlers
{
    GenericMessageHandler::GenericMessageHandler(message_processor_t processor)
    {
        this->processor = processor;
    }

    ProcessingResult GenericMessageHandler::process(PeerMessage *message)
    {
        LOG_DEBUG("[GenericMessageHandler] Delegating message handling");
        return this->processor(message);
    }
}