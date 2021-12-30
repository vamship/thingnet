#include <Arduino.h>

#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "esp_now_node.h"
#include "message_handler.h"
#include "peer_message_handler.h"
#include "server_node_manager.h"

namespace thingnet::node_managers
{
    const int __SERVER_NODE_ADVERTISE_DURATION = 1000;
    const int __SERVER_NODE_PRUNE_DURATION = 1000;

    const u8 __BROADCAST_PEER[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ServerNodeManager::ServerNodeManager(EspNowNode *node)
    {
        this->node = node;
        this->peer_count = 0;
        this->advertise_timer = new Timer(__SERVER_NODE_ADVERTISE_DURATION,
                                          true);
        this->prune_timer = new Timer(__SERVER_NODE_PRUNE_DURATION, true);
    }

    ProcessingResult ServerNodeManager::process(PeerMessage *message)
    {
        LOG_DEBUG("Checking if peer has previously been registered");
        for (int index = 0; index < this->peer_count; index++)
        {
            if (memcmp(this->peer_list[index], message->sender, 6) == 0)
            {
                LOG_INFO("Peer already registered");
                return ProcessingResult::handled;
            }
        }

        LOG_DEBUG("Adding peer to list");
        memcpy(this->peer_list[this->peer_count], message->sender, 6);
        this->peer_count++;

        LOG_DEBUG("Configuring peer");
        ASSERT_OK(register_peer(message->sender, ESP_NOW_ROLE_COMBO));

        LOG_DEBUG("Setting up message handler for peer");
        this->node->add_handler(new PeerMessageHandler(message->sender));

        LOG_INFO("New peer registered");
        return ProcessingResult::handled;
    }

    int ServerNodeManager::init()
    {
        LOG_INFO("Initializing server node manager");
        this->advertise_timer->start();
        this->prune_timer->start();

        ASSERT_OK(register_peer((u8 *)__BROADCAST_PEER, ESP_NOW_ROLE_CONTROLLER));

        LOG_INFO("Server node manager initialized");
        return RESULT_OK;
    }

    int ServerNodeManager::update()
    {
        if (this->advertise_timer->is_complete())
        {
            LOG_INFO("Advertising server to peers");
            u8 payload[] = {0x01};
            esp_now_send((u8 *)__BROADCAST_PEER, payload, 1);
        }

        if (this->prune_timer->is_complete())
        {
            LOG_INFO("Pruning inactive peers");
            //     // u8 payload[250];
            //     // if (this->peer_count > 0)
            //     // {
            //     //     LOG_DEBUG("Preparing message");
            //     //     payload[0] = 5; // Length of the message (Hello)
            //     //     strcpy((char *)payload + 1, "Hello");
            //     // }
            //     // else
            //     // {
            //     //     LOG_DEBUG("No peers registered");
            //     // }

            //     // for (u8 peer_index = 0; peer_index < this->peer_count; peer_index++)
            //     // {
            //     //     u8 *peer_mac = this->peer_list[peer_index];

            //     //     LOG_DEBUG("Sending message to peer");
            //     //     esp_now_send(peer_mac, (u8 *)&payload, sizeof(payload));

            //     //     LOG_DEBUG("Message sent");
            //     // }
        }

        return RESULT_OK;
    }
}