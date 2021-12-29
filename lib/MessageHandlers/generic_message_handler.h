
#ifndef __GENERIC_MESSAGE_HANDLER_H
#define __GENERIC_MESSAGE_HANDLER_H

#include <Arduino.h>
#include "message_handler.h"

namespace thingnet::message_handlers
{

    /**
     * @brief A callback function that can be used to handle messages from
     * unregistered peers.
     */
    typedef ProcessingResult (*message_processor_t)(PeerMessage *message);

    /**
     * @brief Processes a message received from another node.
     */
    class GenericMessageHandler : public MessageHandler
    {
    private:
        message_processor_t processor;

    public:
        /**
         * @brief Construct a new Generic Message Handler object
         * 
         * @param processor A processor function that will perform the
         *        actual message processing.
         */
        GenericMessageHandler(message_processor_t processor);

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