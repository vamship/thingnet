#ifndef __NODE_PROFILE_H
#define __NODE_PROFILE_H

#include <Arduino.h>

#include "timer.h"
#include "node.h"
#include "message_handler.h"
#include "peer.h"
#include "event_emitter.h"

using namespace thingnet::message_handlers;
using namespace thingnet::utils;
using namespace thingnet::peers;

namespace thingnet
{
    const int __MAX_PEER_COUNT = 50;

    /**
     * @brief Information about a peer, passed to listeners when a peer is
     * added or removed from the profile.
     */
    typedef struct PeerListEventData
    {
        u8 peer_mac_address[6];

        PeerListEventData()
        {
            memset(peer_mac_address, 0, 6);
        }

        PeerListEventData(Peer *peer)
        {
            peer->read_mac_address(peer_mac_address);
        }
    } PeerListEventData;

    /**
     * @brief Base class for node profile entities. Node profiles are designed
     * to serve as a profile for the node, governing its behavior as a server
     * or a peer.
     * 
     * Node profiles should be designed to function as the default message
     * handlers for a node, dealing with messages that are not handled by
     * upstream handlers.
     * 
     * This class can be extended and customized for specific node profiles,
     * such as servers, clients, etc.
     */
    class NodeProfile : public MessageHandler
    {
    private:
        Timer *prune_timer;
        u32 prune_period;
        EventEmitter<PeerListEventData> *peer_added;
        EventEmitter<PeerListEventData> *peer_removed;

    protected:
        bool is_initialized;
        Node *node;
        Peer *peer_list[__MAX_PEER_COUNT];
        u8 peer_count;

        /**
         * @brief Creates a new peer object, typically when establishing a
         * connection to a server or client. Child classes can override this
         * implementation to create different peers based on specific
         * requirements.
         * 
         * @param message A pointer to the message that was received from the
         * peer.
         * @return Peer* Pointer to a newly created peer object.
         */
        virtual Peer *create_peer(PeerMessage *message) = 0;

    public:
        /**
         * @brief Construct a new Node Profile object
         * 
         * @param node Reference to the node object that will be used for low
         * level peer communication and management.
         */
        NodeProfile(Node *node);

        /**
         * @brief Sets the inactive peer prune duration for the node.
         * 
         * @param timeout The period at which the profile checks for inactive
         * peers.
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        int set_prune_period(u32 timeout);

        /**
         * @brief Gets the number of active peers associated with the profile.
         * 
         * @return int The number of active peers
         */
        int get_peer_count();

        /**
         * @brief Processes a message and returns a result that reflects the
         * result of the processing. If the incoming message represents a new
         * connection, a peer will be registered and added to the internal list of
         * peers.
         *
         * @param message A pointer to the message that the handler will
         * receive.
         * @return ProcessingResult::handled If the message was completely
         * handled by the processor and no further processing is required.
         * @return ProcessingResult::chain If the message was processed
         * successfully, but can be handled by other processors in the
         * chain.
         * @return ProcessingResult::error If there was an error processing the
         * message.
         */
        virtual ProcessingResult process(PeerMessage *message);

        /**
         * @brief Destroy the Node Profile object. This is a virtual method
         * that should be implemented by child classes as appropriate.
         */
        virtual ~NodeProfile();

        /**
         * @brief Allows the node profile to initialize itself. This method
         * will be called once at the start of the program.
         * 
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        virtual int init();

        /**
         * @brief Allows the node profile to update itself. This method will
         * typically be called from within a processing loop, and must be
         * non blocking.
         * 
         * @return int A non success value will be returned if the add operation
         * resulted in an error. See error codes for more information.
         */
        virtual int update();

        /**
         * @brief Returns a reference to the peer added event, to which
         * additional listeners may be attached.
         * 
         * @return Reference to the peer added event.
         */
        const Event<PeerListEventData> *get_peer_added_event();

        /**
         * @brief Returns a reference to the peer removed event, to which
         * additional listeners may be attached.
         * 
         * @return Reference to the peer removed event.
         */
        const Event<PeerListEventData> *get_peer_removed_event();
    };
}

#endif