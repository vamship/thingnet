#ifndef __NODE_MANAGER_H
#define __NODE_MANAGER_H

#include <Arduino.h>

#include "message_handler.h"
#include "peer.h"
#include "timer.h"

using namespace thingnet::message_handlers;
using namespace thingnet::utils;
using namespace thingnet::peers;

// Forward declaration to prevent circular references.
// See: https://stackoverflow.com/questions/625799/resolve-build-errors-due-to-circular-dependency-amongst-classes
namespace thingnet
{
    class EspNowNode;
}

namespace thingnet::node_managers
{
    const int __MAX_PEER_COUNT = 50;

    /**
     * @brief Base class for node manager entities. Node managers are
     * designed to manage the basic operation of the node - adding peers,
     * updating them at regular intervals, etc.
     * 
     * Node managers should be designed to function as the default message
     * handlers for a node, dealing with messages that are not handled by
     * upstream handlers.
     * 
     * This class can be extended and customized for specific node profiles,
     * such as servers, clients, etc.
     */
    class NodeManager : public MessageHandler
    {
    private:
        Timer *prune_timer;

    protected:
        EspNowNode *node;
        Peer *peer_list[__MAX_PEER_COUNT];
        u8 peer_count;

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
         * @brief Construct a new Node Manager object
         * 
         * @param node Reference to the node object that will be used for low
         *        level peer communication and management.
         */
        NodeManager(EspNowNode *node);

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
        virtual ProcessingResult process(PeerMessage *message);

        /**
         * @brief Destroy the Node Manager object. This is a virtual method
         * that should be implemented by child classes as appropriate.
         */
        virtual ~NodeManager();

        /**
         * @brief Allows the node manager to initialize itself. This method
         * will be called once at the start of the program.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int init();

        /**
         * @brief Allows the node manager to update itself. This method will
         * typically be called from within a processing loop, and must be
         * non blocking.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int update();
    };
}

#endif