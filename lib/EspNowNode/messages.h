#ifndef __MESSAGES_H
#define __MESSAGES_H

#include <Arduino.h>

namespace thingnet
{
    /**
     * @brief A general acknowledgement that a message has been received by a
     * peer. The interpretation of the acknowledgement is left to the individual
     * peers that are exchanging messages.
     */
    const u8 MSG_TYPE_ACK = 0x01;

    /**
     * @brief A general rejection notification from a peer. The interpretation
     * of the acknowledgement is left to the individual peers that are
     * exchanging messages.
     */
    const u8 MSG_TYPE_NACK = 0x02;

    /**
     * @brief Server advertisement message, typically follwed by the mac address
     * of the server node.
     */
    const u8 MSG_TYPE_ADVERTISEMENT = 0x10;

    /**
     * @brief Connect message sent by a peer to a server node.
     */
    const u8 MSG_TYPE_CONNECT = 0x11;

    /**
     * @brief A heartbeat message sent by a peer.
     */
    const u8 MSG_TYPE_HEARTBEAT = 0x12;

    /**
     * @brief General data message from a peer to a server node.
     */
    const u8 MSG_TYPE_DATA = 0x13;

    /**
     * @brief The boundary (inclusive) for all reserved messages.
     */
    const u8 MSG_RESERVED_BOUNDARY = 0x7F;

    /**
     * @brief Data structure for messages sent between nodes.
     */
    typedef struct MessagePayload
    {
        u8 type;
        u16 message_id;
        u8 body[247];
    } MessagePayload;

    /**
     * @brief Data structure for message recevied from a remote peer.
     */
    typedef struct PeerMessage
    {
        u8 sender[6];
        MessagePayload payload;
    } PeerMessage;

}

#endif