#include <Arduino.h>

#include "log.h"
#include "error_codes.h"
#include "peer.h"

namespace thingnet::peers
{
    Peer::Peer()
    {
        // Nothing to do here
    }

    Peer::~Peer()
    {
        // Nothing to do here
    }

    int Peer::update()
    {
        return RESULT_OK;
    }
}