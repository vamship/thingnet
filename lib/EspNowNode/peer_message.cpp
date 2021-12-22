#include <Arduino.h>

#include "peer_message.h"
#include "log.h"
#include "format_utils.h"

MessageProcessor::MessageProcessor() {}

bool MessageProcessor::can_process(PeerMessage *message) {
    return true;
}

ProcessingResult MessageProcessor::process(PeerMessage *message) {
  LOG_DEBUG("Processing message");
  return ProcessingResult::handled;
}