#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "node.h"
#include "client_node_profile.h"
#include "basic_peer.h"

using namespace thingnet::peers;
using namespace thingnet::utils;

static Logger *logger = new Logger("client-prof");

namespace thingnet
{
    static const int __DEFAULT_UPDATE_PERIOD = 10000;

    ClientNodeProfile::ClientNodeProfile(EspNowNode *node) : NodeProfile(node)
    {
        this->update_period = __DEFAULT_UPDATE_PERIOD;
        this->update_timer = 0;
    }

    int ClientNodeProfile::set_update_period(u32 timeout)
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node profile has not been initialized");
            return ERR_NODE_PROFILE_NOT_INITIALIZED;
        }

        this->update_period = timeout;

        return RESULT_OK;
    }

    int ClientNodeProfile::init()
    {
        ASSERT_OK(NodeProfile::init());

        LOG_TRACE(logger, "Starting update timer");
        this->update_timer = new Timer(this->update_period, true);
        this->update_timer->start();

        LOG_TRACE(logger, "Peer node manager initialized");
        return RESULT_OK;
    }

    int ClientNodeProfile::update()
    {
        ASSERT_OK(NodeProfile::update());

        if (this->update_timer->is_complete())
        {
            LOG_DEBUG(logger, "Updating [%d] peers", this->peer_count);
            for (u8 index = 0; index < this->peer_count; index++)
            {
                LOG_TRACE(logger, "Updating peer [%d]", index);
                this->peer_list[index]->update();
            }
        }

        return RESULT_OK;
    }

    Peer *ClientNodeProfile::create_peer(PeerMessage *message)
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node profile has not been initialized");
            return 0;
        }

        if (message->payload.type != MSG_TYPE_ADVERTISEMENT)
        {
            LOG_DEBUG(logger, "Unexpected message [%02x] from [%s]. Ignoring.",
                      message->payload.type,
                      LOG_FORMAT_MAC(message->sender));

            return 0;
        }
        u8 mac_addr[6];
        memcpy(mac_addr, message->payload.body, 6);
        LOG_DEBUG(logger, "Advertisement message received from [%s] for [%s]",
                 LOG_FORMAT_MAC(message->sender),
                 LOG_FORMAT_MAC(mac_addr));

        LOG_DEBUG(logger, "Sending connect message");
        MessagePayload payload(MSG_TYPE_CONNECT);
        this->node->send_message(mac_addr, &payload, 0);

        return new BasicPeer(this->node, mac_addr);
    }
}