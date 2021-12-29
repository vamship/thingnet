#ifndef __SERVER_NODE_MANAGER_H
#define __SERVER_NODE_MANAGER_H

#include <Arduino.h>
#include "esp_now_node.h"
#include "message_handler.h"
#include "node_manager.h"
#include "timer.h"

using namespace thingnet::message_handlers;

namespace thingnet::node_managers
{
    /**
     * @brief A node manager implementation for server nodes. Provides basic server
     * functions such as dynamic peer registration, server advertisement,
     * disconnected peer pruning, etc.
     */
    class ServerNodeManager : public NodeManager
    {
    private:
        EspNowNode *node;
        u8 peer_list[255][6];
        u8 peer_count;
        Timer *advertise_timer;
        Timer *prune_timer;

    public:
        /**
         * @brief Construct a new Server Node Manager object
         */
        ServerNodeManager(EspNowNode *node);

        /**
         * @brief Processes a message and returns a result that reflects the
         * result of the processing. If the incoming message represents a new
         * connection, a peer will be registered and added to the internal list of
         * peers.
         *
         * @param message A pointer to the message that the handler will
         *        receive.
         * @return ProcessingResult::handled If the message was completely
         *         handled by the processor and no further processing is
         *         required.
         * @return ProcessingResult::chain If the message was processed
         *         successfully, but can be handled by other processors in the
         *         chain.
         * @return ProcessingResult::error If there was an error processing the
         *         message.
         */
        ProcessingResult process(PeerMessage *message);

        /**
         * @brief Initializes the node manager by starting the appropriate timers.
         * 
         * @return int A non zero value will be returned if the initialization was
         * not successful.
         */
        int init();

        /**
         * @brief Loops through each registered peer and allows the peer to perform
         * update actions. Additionally, performs basic housekeeping such as pruning
         * peer references that are no longer active, etc.
         * 
         * @return int A non zero value will be returned if the update does not
         * complete successfully
         */
        int update();
    };
}

#endif