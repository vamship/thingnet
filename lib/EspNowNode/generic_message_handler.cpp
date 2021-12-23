#include <Arduino.h>

#include "log.h"
#include "format_utils.h"
#include "message_handler.h"
#include "generic_message_handler.h"

GenericMessageHandler::GenericMessageHandler(message_processor_t processor)
{
    this->processor = processor;
}

ProcessingResult GenericMessageHandler::process(PeerMessage *message)
{
    return this->processor(message);
}