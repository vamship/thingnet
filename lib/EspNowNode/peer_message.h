#ifndef __PEER_MESSAGE_H
#define __PEER_MESSAGE_H

#include <Arduino.h>

/**
 * @brief Data structure for message recevied from a remote peer.
 */
typedef struct PeerMessage
{
    u8 sender[6];
    char text[32];
} PeerMessage;

/**
 * @brief An enumeration of possible statuses returned by a handler after
 * processing messages from a peer node.
 */
enum ProcessingResult
{
    /**
   * @brief The message was successfully processed, and processing should be
   * stopped.
   */
    handled,

    /**
   * @brief The message was successfully processed, but can be processed by
   * other processors as well.
   */
    chain,

    /**
   * @brief There was an error processing the message.
   */
    error
};

/**
 * @brief Processes a message received from another node.
 */
class MessageHandler
{
public:
    /**
   * @brief Construct a new message handler object
   */
    MessageHandler();

    /**
   * @brief Returns a boolean value that determines whether or not the
   * handler can/will handle the message.
   *
   * @param message A pointer to the message that the handler will
   *        receive.
   * @return true If the handler wants to handle the message.
   * @return false If the handler does not want to handle the message.
   */
    bool can_handle(PeerMessage *message);

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
   *         successfully, but can be handled by other processors in the
   *         chain.
   * @return ProcessingResult::error If there was an error processing the
   *         message.
   */
    ProcessingResult process(PeerMessage *message);
};
#endif