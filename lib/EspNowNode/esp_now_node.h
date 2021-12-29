#ifndef __ESP_NOW_NODE_H
#define __ESP_NOW_NODE_H

#include <Arduino.h>
#include <espnow.h>
#include "node_manager.h"
#include "message_handler.h"

using namespace thingnet::node_managers;

namespace thingnet
{

    /**
     * @brief Represents a node that can communicate using ESP-NOW
     */
    class EspNowNode
    {
    private:
        u8 sta_mac_address[6];
        u8 ap_mac_address[6];
        bool is_initialized;
        NodeManager *manager;

        EspNowNode();
        ~EspNowNode();

    public:
        /**
         * @brief Get the instance object
         *
         * @return EspNowNode&
         */
        static EspNowNode &get_instance();

        /**
         * @brief Initializes the node and returns a non-zero value if the
         * initialization fails.
         *
         * @return int A non zero value will be returned if the initialization
         *         fails.
         */
        int init();

        /**
         * @brief Adds a message handler to the end of the list of available
         * handlers for the node. 
         * 
         * @param handler The handler to add
         * @return int A non zero value will be returned if the add operation
         *         resulted in an error.
         */
        int add_handler(MessageHandler *handler);

        /**
         * @brief Sets the node manager for the current node. The node manager
         * is responsible for:
         * 1. Handling messages not handled by handlers in the chain
         * 2. Managing connections to other nodes
         * 3. Allowing individual peer controllers to perform updates
         * 
         * @param manager The manager to add
         * @return int A non zero value will be returned if the add operation
         *         resulted in an error.
         */
        int set_node_manager(NodeManager *manager);

        /**
         * @brief Matches the input buffer against the mac address of the node. 
         * 
         * @param input_mac The mac address to compare with the current node.
         * @return true If the input mac address matches the node's mac address
         * @return false If the input mac address odes not match, or if the node
         *         has not been initialized.
         */
            bool has_mac_address(u8 *input_mac);

        /**
         * @brief Allows the node to update itself. This method will typically
         * be called from within a processing loop, and must be non blocking.
         * 
         * @return int A non zero value will be returned if the update did not
         *         work as expected.
         */
        int update();

        // Singleton implementation.
        // See: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
        EspNowNode(EspNowNode const &) = delete;
        void operator=(EspNowNode const &) = delete;
    };
}

#endif
