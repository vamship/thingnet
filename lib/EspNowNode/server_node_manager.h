#ifndef __SERVER_NODE_MANAGER_H
#define __SERVER_NODE_MANAGER_H

#include <Arduino.h>

#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "peer.h"
#include "node_profile.h"
#include "timer.h"

using namespace thingnet::message_handlers;
using namespace thingnet::utils;
using namespace thingnet::peers;

namespace thingnet
{
    /**
     * @brief A node manager implementation for server nodes. Provides basic
     * server functions such as dynamic peer registration, server advertisement,
     * disconnected peer pruning, etc.
     */
    class ServerNodeManager : public NodeProfile
    {
    private:
        Timer *advertise_timer;

    public:
        /**
         * @brief Construct a new Server Node Manager object
         */
        ServerNodeManager(EspNowNode *node);

        /**
         * @brief Initializes the node manager by starting the appropriate
         * timers.
         * 
         * @return int A non success value will be returned if the init
         * operation resulted in an error. See error codes for more information.
         */
        int init();

        /**
         * @brief Loops through each registered peer and allows the peer to
         * perform update actions. Additionally, performs basic housekeeping
         * such as pruning peer references that are no longer active, etc.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int update();
    };
}

#endif