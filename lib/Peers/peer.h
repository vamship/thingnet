#ifndef __PEER_H
#define __PEER_H

#include <Arduino.h>

#include "messages.h"
#include "message_handler.h"

using namespace thingnet::message_handlers;

namespace thingnet::peers
{
    /**
     * @brief Represents a remote peer that is connected to the current node.
     */
    class Peer : public MessageHandler
    {
    private:
        u8 peer_mac_address[6];
    public:
        /**
         * @brief Construct a new basic peer object. Sets the timeout to a
         *        default value of 300s.
         * 
         * @param peer_mac_address The mac address of the peer that is
         *        represented by this object
         */
        Peer(u8 *peer_mac_address);

        /**
         * @brief Destroy the Peer object
         */
        virtual ~Peer();

        /**
         * @brief Copies the mac address of the current peer into the given
         * buffer.
         * 
         * @param buffer A buffer into which the mac address will be copied.
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int read_mac_address(u8 *buffer);

        /**
         * @brief Returns true only if the message is from a peer that this
         * handler is configured for.
         *
         * @param message A pointer to the message that the handler will
         *        receive.
         * @return true If the handler wants to handle the message.
         * @return false If the handler does not want to handle the message.
         */
        virtual bool can_handle(PeerMessage *message);

        /**
         * @brief Allows the peer to run periodic updates.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int update();

        /**
         * @brief Determines whether or not the peer is still active.
         * 
         * @return true If the peer is still active
         * @return false If the peer is no longer active
         */
        virtual bool is_active() = 0;
    };
}

#endif