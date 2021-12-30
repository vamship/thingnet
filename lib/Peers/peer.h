#ifndef __PEER_H
#define __PEER_H

#include <Arduino.h>
#include "message_handler.h"

using namespace thingnet::message_handlers;

namespace thingnet::peers
{
    /**
     * @brief Represents a remote peer that is connected to the current node.
     */
    class Peer : public MessageHandler
    {
    public:
        /**
         * @brief Construct a new Peer object
         */
        Peer();

        /**
         * @brief Destroy the Peer object
         */
        virtual ~Peer();

        /**
         * @brief Determines whether or not the peer is still active.
         * 
         * @return true If the peer is still active
         * @return false If the peer is no longer active
         */
        virtual bool is_active() = 0;

        /**
         * @brief Allows the peer to run periodic updates.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int update();
    };
}

#endif