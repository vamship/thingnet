#ifndef __NODE_H
#define __NODE_H

#include <Arduino.h>
#include <espnow.h>

#include "timer.h"
#include "messages.h"

// Forward declaration to prevent circular references.
// See: https://stackoverflow.com/questions/625799/resolve-build-errors-due-to-circular-dependency-amongst-classes
namespace thingnet
{
    class NodeProfile;
}

namespace thingnet::message_handlers
{
    class MessageHandler;
}

using namespace thingnet::message_handlers;

namespace thingnet
{

    /**
     * @brief Represents a node that can communicate using ESP-NOW
     */
    class Node
    {
    private:
        u8 sta_mac_address[6];
        u8 ap_mac_address[6];
        u16 message_id;
        bool is_initialized;
        NodeProfile *profile;

        Node();
        ~Node();

    public:
        /**
         * @brief Get the instance object
         *
         * @return EspNowNode&
         */
        static Node &get_instance();

        /**
         * @brief Initializes the node and returns a non-zero value if the
         * initialization fails.
         *
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int init();

        /**
         * @brief Generates and returns a new message id value. This method will
         * always return zero if the node has not been initialized.
         * 
         * @return u16 The generated message id.
         */
        u16 get_next_message_id();

        /**
         * @brief Gets the node profile currently associated with the node.
         * 
         * @return NodeProfile* A pointer to the current node profile. A null
         * value will be returned if the node profile has not been configured.
         */
        NodeProfile *get_profile();

        /**
         * @brief Adds a message handler to the end of the list of available
         * handlers for the node. 
         * 
         * @param handler The handler to add
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int add_handler(MessageHandler *handler);

        /**
         * @brief Removes the handler from the list of handlers and compacts the
         * handler list.
         * 
         * @param handler The handler to remove
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int remove_handler(MessageHandler *handler);

        /**
         * @brief Sets the node profile for the current node. The node profile
         * is responsible for:
         * 1. Handling messages not handled by handlers in the chain
         * 2. Managing connections to other nodes
         * 3. Allowing individual peer controllers to perform updates
         * 
         * @param profile The profile to set
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int set_node_profile(NodeProfile *profile);

        /**
         * @brief Matches the input buffer against the mac address of the node. 
         * 
         * @param input_mac The mac address to compare with the current node.
         * @return true If the input mac address matches the node's mac address
         * @return false If the input mac address odes not match, or if the node
         * has not been initialized.
         */
        bool has_mac_address(u8 *input_mac);

        /**
         * @brief Allows the node to update itself. This method will typically
         * be called from within a processing loop, and must be non blocking.
         * 
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int update();

        /**
         * @brief Copies the mac address of the node into the given * buffer.
         * 
         * @param buffer A buffer into which the mac address will be copied.
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int read_mac_address(u8 *buffer);

        /**
         * @brief Registers a new peer with the node. The peer will not be added if
         * it has already been registered.
         * 
         * @param peer_address The mac address of the peer.
         * @param role The role to register the peer as.
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int register_peer(u8 *peer_address, esp_now_role role);

        /**
         * @brief Unregisters an existing peer from the node. This method will have
         * no effect if the peer has not been previously registered.
         * 
         * @param peer_address The mac address of the peer.
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int unregister_peer(u8 *peer_address);

        /**
         * @brief Sends a message to the specified peer.
         * 
         * @param destination The mac address of the peer
         * @param payload A pointer to the message payload
         * @param data_size The length of the payload, not including headers
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int send_message(u8 *destination, MessagePayload *payload, u8 data_size);

        // Singleton implementation.
        // See: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
        Node(Node const &) = delete;
        void operator=(Node const &) = delete;
    };
}

#endif
