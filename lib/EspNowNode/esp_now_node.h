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
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int init();

        /**
         * @brief Adds a message handler to the end of the list of available
         * handlers for the node. 
         * 
         * @param handler The handler to add
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int add_handler(MessageHandler *handler);

        /**
         * @brief Removes the handler from the list of handlers and compacts the
         * handler list.
         * 
         * @param handler The handler to remove
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int remove_handler(MessageHandler *handler);

        /**
         * @brief Sets the node manager for the current node. The node manager
         * is responsible for:
         * 1. Handling messages not handled by handlers in the chain
         * 2. Managing connections to other nodes
         * 3. Allowing individual peer controllers to perform updates
         * 
         * @param manager The manager to add
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
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
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int update();

        // Singleton implementation.
        // See: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
        EspNowNode(EspNowNode const &) = delete;
        void operator=(EspNowNode const &) = delete;
    };

    /**
     * @brief Registers a new peer with the node. The peer will not be added if
     * it has already been registered.
     * 
     * @param peer_address The mac address of the peer.
     * @param role The role to register the peer as.
     * @return int A non success value will be returned if the add operation
     *         resulted in an error. See error codes for more information.
     */
    int register_peer(u8 *peer_address, esp_now_role role);

    /**
     * @brief Unregisters an existing peer from the node. This method will have
     * no effect if the peer has not been previously registered.
     * 
     * @param peer_address The mac address of the peer.
     * @return int A non success value will be returned if the add operation
     *         resulted in an error. See error codes for more information.
     */
    int unregister_peer(u8* peer_address);
}

#endif
