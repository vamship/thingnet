#ifndef __SERVER_NODE_PROFILE_H
#define __SERVER_NODE_PROFILE_H

#include <Arduino.h>

#include "messages.h"
#include "node.h"
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
    protected:
        /**
         * @brief Creates a new peer object if a connect message is received
         * from the peer.
         * 
         * @param message A pointer to the message that was received from the
         *        peer.
         * @return Peer* Pointer to a newly created peer object.
         */
        virtual Peer *create_peer(PeerMessage *message);

    public:
        /**
         * @brief Construct a new Server Node Profile object
         */
        ServerNodeProfile(Node *node);

        /**
         * @brief Broadcasts an advertisement message to all peers.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int advertise();

        /**
         * @brief Initializes the node profile by starting the appropriate
         * timers.
         * 
         * @return int A non success value will be returned if the init
         * operation resulted in an error. See error codes for more information.
         */
        int init();
    };
}

#endif