#ifndef __SERVER_NODE_PROFILE_H
#define __SERVER_NODE_PROFILE_H

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
     * @brief A node profile implementation for server nodes. Provides basic
     * server functions such as dynamic peer registration, server advertisement,
     * disconnected peer pruning, etc.
     */
    class ServerNodeProfile : public NodeProfile
    {
    private:
        Timer *advertise_timer;
        u32 advertise_period;

    public:
        /**
         * @brief Construct a new Server Node Profile object
         */
        ServerNodeProfile(EspNowNode *node);

        /**
         * @brief Sets the advertisement period for the node.
         * 
         * @param timeout The period at which the profile advertises itself to
         *        peers.
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int set_advertise_period(u32 timeout);

        /**
         * @brief Initializes the node profile by starting the appropriate
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