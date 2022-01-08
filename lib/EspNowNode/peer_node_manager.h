#ifndef __PEER_NODE_MANAGER_H
#define __PEER_NODE_MANAGER_H

// WEIRD BEHAVIOR: For some reason naming this file "client_node_manager.h"
// seems to cause compilation issues. I cannot explain it, but that's what the
// preliminary evidence points to. Just to keep things moving, are just going to
// adopt the name change for now.

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
     * @brief A node manager implementation for peer nodes. Automatically
     * registers itself with a server that is advertising itself, and
     * disconnects if the server becomes inactive.
     */
    class PeerNodeManager : public NodeManager
    {
    private:
        Timer *message_timer;

    protected:
        /**
         * @brief Creates a new peer object, typically when establishing a
         * connection to a server or peer. Child classes can override this
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
         * @brief Construct a new Peer Node Manager object
         */
        PeerNodeManager(EspNowNode *node);

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