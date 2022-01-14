#ifndef __CLIENT_NODE_PROFILE_H
#define __CLIENT_NODE_PROFILE_H

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
     * @brief A node profile implementation for client nodes. Automatically
     * registers itself with a server that is advertising itself, and
     * disconnects if the server becomes inactive.
     */
    class ClientNodeProfile : public NodeProfile
    {
    private:
        Timer *update_timer;
        u32 update_period;

    protected:
        /**
         * @brief Creates a new peer object, typically when establishing a
         * connection to a server or client. Child classes can override this
         * implementation to create different peers based on specific
         * requirements.
         * 
         * @param message A pointer to the message that was received from the
         *        peer.
         * @return Peer* Pointer to a newly created peer object.
         */
        virtual Peer *create_peer(PeerMessage *message);

    public:
        /**
         * @brief Construct a new Client Node Profile object
         */
        ClientNodeProfile(EspNowNode *node);

        /**
         * @brief Sets the peer update duration for the node.
         * 
         * @param timeout The period at which peers will be updated, allowing
         *        them to execute the functionality defined within their
         *        update() method.
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int set_update_period(u32 timeout);

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