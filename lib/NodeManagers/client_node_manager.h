#ifndef __CLIENT_NODE_MANAGER_H
#define __CLIENT_NODE_MANAGER_H

#include <Arduino.h>

#include "timer.h"
#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "peer.h"
#include "node_manager.h"

using namespace thingnet::message_handlers;
using namespace thingnet::utils;
using namespace thingnet::peers;

namespace thingnet::node_managers
{
    /**
     * @brief A node manager implementation for client nodes. Automatically
     * registers itself with a server that is advertising itself, and
     * disconnects if the server becomes inactive.
     */
    class ClientNodeManager : public NodeManager
    {
    private:
        Timer *message_timer;

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
         * @brief Construct a new Client Node Manager object
         */
        ClientNodeManager(EspNowNode *node);

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
         * perform * update actions. Additionally, performs basic housekeeping
         * such as pruning peer references that are no longer active, etc.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int update();
    };
}

#endif