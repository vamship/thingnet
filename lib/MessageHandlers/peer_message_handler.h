#ifndef __PEER_MESSAGE_HANDLER_H
#define __PEER_MESSAGE_HANDLER_H

#include <Arduino.h>

#include "messages.h"
#include "message_handler.h"

namespace thingnet::message_handlers
{
    /**
     * @brief Processes a message received from another node.
     */
    class PeerMessageHandler : public MessageHandler
    {
    private:
        u8 peer_mac_address[6];

    public:
        /**
         * @brief Construct a new Peer Message Handler object
         * 
         * @param peer_mac_address The mac address of the peer whose messages
         *        will be handled by this handler.
         */
        PeerMessageHandler(u8 *peer_mac_address);

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
        ProcessingResult process(PeerMessage *message);
    };
}

#endif