#ifndef __MESSAGES_H
#define __MESSAGES_H

#include <Arduino.h>

namespace thingnet
{

    /**
     * @brief Server advertisement message, typically follwed by the mac address
     * of the server node.
     */
    const u8 MSG_TYPE_ADVERTISEMENT = 0x01;

    /**
     * @brief Connect message sent by a peer to a server node.
     */
    const u8 MSG_TYPE_CONNECT = 0x02;

    /**
     * @brief Connection acceptance message sent by a server to a peer node.
     */
    const u8 MSG_TYPE_CONNECT_ACK = 0x02;

    /**
     * @brief Connection declined message sent by a server to a peer node.
     */
    const u8 MSG_TYPE_CONNECT_NACK = 0x03;

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
        u8 body[249];
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