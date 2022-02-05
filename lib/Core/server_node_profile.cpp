#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "node.h"
#include "message_handler.h"
#include "basic_peer.h"
#include "server_node_profile.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("server-prof");

namespace thingnet
{
    static const u8 __BROADCAST_PEER[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ServerNodeProfile::ServerNodeProfile(Node *node) : NodeProfile(node)
    {
    }

    int ServerNodeProfile::init()
    {
        ASSERT_OK(NodeProfile::init());

        LOG_TRACE(logger, "Registering broadcast peer");
        ASSERT_OK(this->node->register_peer((u8 *)__BROADCAST_PEER,
                                            ESP_NOW_ROLE_CONTROLLER));

        LOG_TRACE(logger, "Server node profile initialized");
        return RESULT_OK;
    }

    int ServerNodeProfile::advertise()
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node profile has not been initialized");
            return ERR_NODE_PROFILE_NOT_INITIALIZED;
        }

        LOG_TRACE(logger, "Advertising server to peers");
        MessagePayload payload(MSG_TYPE_ADVERTISEMENT);
        this->node->read_mac_address(payload.body);

        this->node->send_message((u8 *)__BROADCAST_PEER, &payload, 6);
        return RESULT_OK;
    }

    Peer *ServerNodeProfile::create_peer(PeerMessage *message)
    {
        if (message->payload.type != MSG_TYPE_CONNECT)
        {
            LOG_DEBUG(logger, "Unexpected message [%02x] from [%s]. Ignoring.",
                      message->payload.type,
                      LOG_FORMAT_MAC(message->sender));

            return 0;
        }

        LOG_DEBUG(logger, "[CONNECT] received from [%s]",
                  LOG_FORMAT_MAC(message->sender));

        return new BasicPeer(this->node, message->sender);
    }
}