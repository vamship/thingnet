#include <Arduino.h>
#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "messages.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "basic_peer.h"
#include "node_manager.h"

namespace thingnet::node_managers
{
    const int __NODE_MANAGER_PRUNE_DURATION = 10000;

    NodeManager::NodeManager(EspNowNode *node)
    {
        this->node = node;
        this->peer_count = 0;
        this->prune_timer = new Timer(__NODE_MANAGER_PRUNE_DURATION, true);
    }

    NodeManager::~NodeManager()
    {
        // Nothing to do here.
    }

    Peer *NodeManager::create_peer(PeerMessage *message)
    {
        return new BasicPeer(message->sender, 30000);
    }

    int NodeManager::init()
    {
        LOG_INFO("Starting prune timer");
        this->prune_timer->start();

        return RESULT_OK;
    }

    ProcessingResult NodeManager::process(PeerMessage *message)
    {
        LOG_INFO("Registering a new peer");
        int result = register_peer(message->sender, ESP_NOW_ROLE_COMBO);
        ASSERT_OK(result);

        // Result could be OK or DUPLICATE. Both cases are considered to be
        // non-error. However, we do not want to add another peer reference
        // if the peer already exists.
        if (result == RESULT_OK)
        {
            LOG_DEBUG("Requesting new peer instance");
            Peer *peer = this->create_peer(message);

            if (peer == 0)
            {
                LOG_DEBUG("No peer was created");
            }
            else
            {
                LOG_DEBUG("Registering new peer");

                this->peer_list[this->peer_count] = peer;
                this->peer_count++;

                LOG_DEBUG("Setting up message handler for peer");
                this->node->add_handler(peer);
            }
        }
        else
        {
            LOG_WARN("A peer has already been registered");
        }

        LOG_INFO("Peer registration process completed");
        return ProcessingResult::handled;
    }

    int NodeManager::update()
    {
        if (this->prune_timer->is_complete())
        {
            LOG_INFO("Looking for inactive peers");
            for (u8 peer_index = 0; peer_index < this->peer_count; peer_index++)
            {
                LOG_DEBUG("Checking peer [%d]", peer_index);
                Peer *current_peer = this->peer_list[peer_index];
                if (!current_peer->is_active())
                {
                    LOG_INFO("Peer [%d] is inactive", peer_index);

                    // Removing peer handler
                    this->node->remove_handler(current_peer);

                    // Removing message handler
                    u8 mac_addr[6];
                    current_peer->read_mac_address(mac_addr);
                    ASSERT_OK(unregister_peer(mac_addr));

                    // Destroy the peer and mark the pointer as removed.
                    LOG_DEBUG("Destroying peer [%d] [%s]",
                              peer_index,
                              LOG_FORMAT_MAC(mac_addr));

                    delete current_peer;
                    this->peer_list[peer_index] = 0;
                }
                else
                {
                    LOG_DEBUG("Peer [%d] is active", peer_index);
                }
            }

            LOG_DEBUG("Compacting peer list");
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
            LOG_INFO("Pruned [%d] inactive peers. Peer count [%d]", prune_count, this->peer_count);
        }

        return RESULT_OK;
    }
}
