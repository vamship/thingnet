#include <Arduino.h>

#include "peer_message.h"
#include "log.h"
#include "format_utils.h"

MessageHandler::MessageHandler() {}

bool MessageHandler::can_handle(PeerMessage *message) {
    return true;
}

ProcessingResult MessageHandler::process(PeerMessage *message) {
  LOG_DEBUG("Processing message");
  return ProcessingResult::handled;
}