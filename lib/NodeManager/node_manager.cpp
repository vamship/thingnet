#include <Arduino.h>

#include "error_codes.h"
#include "node_manager.h"

namespace thingnet
{
    namespace node_managers
    {

        NodeManager::NodeManager()
        {
            // Nothing to do here.
        }

        NodeManager::~NodeManager()
        {
            // Nothing to do here.
        }

        int NodeManager::init()
        {
            // Default implementation does nothing.
            return RESULT_OK;
        }
    }
}