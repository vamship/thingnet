#include <Arduino.h>
#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "basic_peer.h"
#include "node_profile.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("prof");

namespace thingnet
{
    static const int __DEFAULT_PRUNE_PERIOD = 300000;

    NodeProfile::NodeProfile(EspNowNode *node)
    {
        this->node = node;
        this->prune_period = __DEFAULT_PRUNE_PERIOD;
        this->peer_count = 0;
        this->prune_timer = 0;
        this->is_initialized = false;
    }

    NodeProfile::~NodeProfile()
    {
        // Nothing to do here.
    }

    int NodeProfile::set_prune_period(u32 timeout)
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node profile has not been initialized");
            return ERR_NODE_PROFILE_NOT_INITIALIZED;
        }

        this->prune_period = timeout;

        return RESULT_OK;
    }

    int NodeProfile::get_peer_count()
    {
        return this->peer_count;
    }

    int NodeProfile::init()
    {
        LOG_TRACE(logger, "Initializing node profile");
        if (this->is_initialized)
        {
            LOG_WARN(logger, "Node profile has already been initialized");
            return RESULT_DUPLICATE;
        }

        LOG_TRACE(logger, "Starting prune timer");
        this->prune_timer = new Timer(this->prune_period, true);
        this->prune_timer->start();

        this->is_initialized = true;

        return RESULT_OK;
    }

    ProcessingResult NodeProfile::process(PeerMessage *message)
    {
        if (!this->is_initialized)
        {
            LOG_ERROR(logger, "Node profile has not been initialized");
            return ProcessingResult::error;
        }

        LOG_TRACE(logger, "Executing node manager message handler");

        // Allow the child class to determine if a new peer needs to be created.
        // A child class can do this by overriding the create_peer() method.
        LOG_TRACE(logger, "Requesting new peer instance");
        Peer *peer = this->create_peer(message);

        if (peer == 0)
        {
            // Child class thinks no peer is required.
            LOG_DEBUG(logger, "No peer was created");
        }
        else
        {
            LOG_TRACE(logger, "New peer created");
            u8 mac_addr[6];
            peer->read_mac_address(mac_addr);

            LOG_DEBUG(logger, "Adding peer [%s] to internal registry",
                      LOG_FORMAT_MAC(mac_addr));

            int result = this->node->register_peer(mac_addr,
                                                   ESP_NOW_ROLE_COMBO);
            ASSERT_OK(result);

            // Result could be OK or DUPLICATE. Both cases are considered to be
            // non-error. However, we do not want to add another peer reference
            // if the peer already exists.
            if (result == RESULT_OK)
            {
                LOG_TRACE(logger, "Configuring peer");
                this->peer_list[this->peer_count] = peer;
                this->peer_count++;

                this->node->add_handler(peer);
            }
            else
            {
                // There is already a peer registered, so delete the one created
                // by the child class
                delete peer;
                LOG_WARN(logger, "A peer has already been registered");
            }
        }

        LOG_TRACE(logger, "Peer registration process completed");
        return ProcessingResult::handled;
    }

    int NodeProfile::update()
    {
        if (!this->is_initialized)
        {
            LOG_ERROR(logger, "Node profile has not been initialized");
            return ERR_NODE_PROFILE_NOT_INITIALIZED;
        }

        if (this->prune_timer->is_complete())
        {
            LOG_DEBUG(logger, "Pruning peer list");
            for (u8 peer_index = 0; peer_index < this->peer_count; peer_index++)
            {
                LOG_TRACE(logger, "Checking peer [%d]", peer_index);
                Peer *current_peer = this->peer_list[peer_index];
                if (!current_peer->is_active())
                {
                    LOG_TRACE(logger, "Peer [%d] is inactive", peer_index);

                    // Removing peer handler
                    this->node->remove_handler(current_peer);

                    // Removing message handler
                    u8 mac_addr[6];
                    current_peer->read_mac_address(mac_addr);
                    ASSERT_OK(this->node->unregister_peer(mac_addr));

                    // Destroy the peer and mark the pointer as removed.
                    LOG_DEBUG(logger, "Destroying peer [%d] [%s]",
                              peer_index,
                              LOG_FORMAT_MAC(mac_addr));

                    delete current_peer;
                    this->peer_list[peer_index] = 0;
                }
                else
                {
                    LOG_TRACE(logger, "Peer [%d] is active", peer_index);
                }
            }

            LOG_TRACE(logger, "Compacting peer list");
            u8 prune_count = 0;
            for (u8 peer_index = 0; peer_index < this->peer_count; peer_index++)
            {
                if (this->peer_list[peer_index] == 0)
                {
                    prune_count++;
                }
                else
                {
                    this->peer_list[peer_index - prune_count] = this->peer_list[peer_index];
                }
            }

            this->peer_count -= prune_count;
            LOG_DEBUG(logger, "Pruned [%d] inactive peers. Peer count [%d]", prune_count, this->peer_count);
        }

        return RESULT_OK;
    }
}
