#ifndef __BASIC_PEER_H
#define __BASIC_PEER_H

#include <Arduino.h>
#include "peer.h"

namespace thingnet::peers
{
    /**
     * @brief A very basic implementation of a remote peer. Does nothing other
     * than track whether or not the remote peer is active based on time elapsed
     * since the last message was received.
     */
    class BasicPeer : public Peer
    {
    private:
        u8 peer_mac_address[6];
        u64 last_message_time;
        u32 timeout;

    public:
        /**
         * @brief Construct a new basic peer object
         * 
         * @param peer_mac_address The mac address of the peer that is
         *        represented by this object
         * @param timeout The number of milliseconds since the last message was
         *         received after which the peer will be deemed to be inactive.
         */
        BasicPeer(u8 *peer_mac_address, u32 timeout);

        /**
         * @brief Construct a new basic peer object. Sets the timeout to a
         *        default value of 300s.
         * 
         * @param peer_mac_address The mac address of the peer that is
         *        represented by this object
         */
        BasicPeer(u8 *peer_mac_address);

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
         * @brief Processes a message and returns a result that reflects the
         * result of the processing.
         *
         * @param message A pointer to the message that the handler will
         *        receive.
         * @return ProcessingResult::handled If the message was completely
         *         handled by the processor and no further processing is
         *         required.
         * @return ProcessingResult::chain If the message was processed
         *         successfully, but can be handled by other processors in
         *         the chain.
         * @return ProcessingResult::error If there was an error processing
         *         the message.
         */
        virtual ProcessingResult process(PeerMessage *message);

        /**
         * @brief Determines whether or not the peer is still active based on
         * the time elapsed since a message was last received from the peer.
         * 
         * @return true If the peer is still active
         * @return false If the peer is no longer active
         */
        virtual bool is_active();
    };
}

#endif