#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"

#include "messages.h"
#include "esp_now_node.h"
#include "node_manager.h"
#include "message_handler.h"
#include "error_codes.h"

namespace thingnet
{
    const u8 __MAX_HANDLER_COUNT = 255;
    MessageHandler *__message_handler_list[__MAX_HANDLER_COUNT];
    MessageHandler *__default_handler = 0;
    u8 __message_handler_count = 0;

    /**
     * @brief Handles data send confirmation.
     *
     * @param mac_addr The mac address of the recipient
     * @param status Whether or not the message was sent successfully
     */
    void __on_data_sent(u8 *mac_addr, u8 status)
    {
        LOG_INFO("Message delivery ack to [%s] [%s]", LOG_FORMAT_MAC(mac_addr), status == 0 ? "ok" : "err");
    }

    /**
     * @brief Handles data received from peers
     *
     * @param mac_addr The mac address of the sender
     * @param data The payload received from the sender
     * @param length The length of the payload
     */
    void __on_data_received(u8 *mac_addr, u8 *data, u8 length)
    {
        LOG_INFO("Message received from peer");

        LOG_DEBUG("Received:[%02x|%02x:%02x]", data[0], data[1], data[2]);

        LOG_DEBUG("Preparing message payload");
        PeerMessage message;
        memcpy(message.sender, mac_addr, 6);
        memcpy(&message.payload.type, data, 1);
        memcpy(&message.payload.message_id, data + 1, 2);
        memcpy(&message.payload.body, data + 3, length - 3);

        bool processing_complete = false;
        LOG_DEBUG("Starting handler chain");
        for (int index = 0; index < __message_handler_count; index++)
        {
            MessageHandler *handler = __message_handler_list[index];
            if (!handler->can_handle(&message))
            {
                LOG_DEBUG("Handler [%d] will not handle message", index);
                continue;
            }

            LOG_DEBUG("Handler [%d] will handle message", index);
            ProcessingResult result = handler->process(&message);

            if (result == ProcessingResult::handled)
            {
                processing_complete = true;
                LOG_DEBUG("Handler chain completed");
                break;
            }
            else if (result == ProcessingResult::error)
            {
                processing_complete = true;
                LOG_WARN("Error processing message by processor [%d]", index);
                break;
            }
        }

        if (!processing_complete)
        {
            LOG_DEBUG("Handler chain is still not complete");
            if (__default_handler != 0)
            {
                LOG_DEBUG("Checking if default handler will process the message");
                if (__default_handler->can_handle(&message))
                {
                    LOG_DEBUG("Invoking default handler");
                    ProcessingResult result = __default_handler->process(&message);

                    if (result == ProcessingResult::error)
                    {
                        LOG_WARN("Error processing message by default handler");
                    }
                }
                else
                {
                    LOG_WARN("Default handler will not handle message");
                }
            }
            else
            {
                LOG_WARN("Default handler has not been set. Skipping.");
            }
        }

        LOG_INFO("Message from peer processed");
    }

    EspNowNode::EspNowNode()
    {
        this->manager = 0;
    }

    EspNowNode::~EspNowNode()
    {
        // Loop through all handlers and delete them.
        for (int index = 0; index < __message_handler_count; index++)
        {
            delete __message_handler_list[index];
        }
        __message_handler_count = 0;
    }

    EspNowNode &EspNowNode::get_instance()
    {
        static EspNowNode instance;
        return instance;
    }

    int EspNowNode::init()
    {
        LOG_INFO("Initializing node");
        if (this->is_initialized)
        {
            LOG_WARN("Node has already been initialized");
            return RESULT_DUPLICATE;
        }

        LOG_DEBUG("Reading current mac address(es)");
        WiFi.macAddress(this->sta_mac_address);
        WiFi.softAPmacAddress(this->ap_mac_address);

        LOG_DEBUG("Setting wifi to station mode");
        WiFi.mode(WIFI_AP_STA);

        LOG_DEBUG("Initializing ESP-NOW")
        if (esp_now_init() != 0)
        {
            LOG_ERROR("Error initializing ESP-NOW");
            return 1;
        }
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

        LOG_DEBUG("Registering send/receive callbacks");
        esp_now_register_send_cb(__on_data_sent);
        esp_now_register_recv_cb(__on_data_received);

        this->is_initialized = true;

        LOG_DEBUG("AP MAC Address : [%s]", LOG_FORMAT_MAC(this->ap_mac_address));
        LOG_DEBUG("STA MAC Address: [%s]", LOG_FORMAT_MAC(this->sta_mac_address));

        LOG_INFO("Node initialized");

        return RESULT_OK;
    }

    u16 EspNowNode::get_next_message_id()
    {
        if (!this->is_initialized)
        {
            LOG_WARN("Node has not been initialized");
            return 0;
        }
        this->message_id++;
        return this->message_id;
    }

    int EspNowNode::add_handler(MessageHandler *handler)
    {
        LOG_INFO("Registering message handler");

        if (!this->is_initialized)
        {
            LOG_ERROR("Node has not been initialized");
            return ERR_NODE_NOT_INITIALIZED;
        }

        if (__message_handler_count >= __MAX_HANDLER_COUNT)
        {
            LOG_ERROR("Cannot add handler - maximum handler limit has been reached");
            return ERR_HANDLER_LIMIT_EXCEEDED;
        }

        // Add handler to the end of the list.
        __message_handler_list[__message_handler_count] = handler;
        __message_handler_count++;

        LOG_INFO("Handler added successfully. Total handlers: [%d]", __message_handler_count);

        return RESULT_OK;
    }

    int EspNowNode::remove_handler(MessageHandler *handler)
    {
        LOG_INFO("Unregistering message handler");

        if (!this->is_initialized)
        {
            LOG_ERROR("Node has not been initialized");
            return ERR_NODE_NOT_INITIALIZED;
        }

        u8 find_count = 0;
        for (u8 handler_index = 0; handler_index < __message_handler_count; handler_index++)
        {
            if (__message_handler_list[handler_index] == handler)
            {
                find_count++;
            }
            else
            {
                __message_handler_list[handler_index - find_count] =
                    __message_handler_list[handler_index];
            }
        }

        if (find_count == 0)
        {
            LOG_WARN("Could not find message handler");
            return RESULT_NO_EXIST;
        }
        __message_handler_count -= find_count;

        LOG_INFO("Handler(s) removed successfully [%d]. Total handlers: [%d]",
                 find_count,
                 __message_handler_count);

        return RESULT_OK;
    }

    int EspNowNode::set_node_manager(NodeManager *manager)
    {
        LOG_INFO("Registering node manager");

        if (!this->is_initialized)
        {
            LOG_ERROR("Node has not been initialized");
            return ERR_NODE_NOT_INITIALIZED;
        }

        if (this->manager != 0)
        {
            LOG_WARN("Node manager has already been registered");
            return RESULT_DUPLICATE;
        }

        this->manager = manager;

        LOG_DEBUG("Configuring default handler");
        __default_handler = manager;

        LOG_INFO("Node manager registered");

        return RESULT_OK;
    }

    bool EspNowNode::has_mac_address(u8 *input_mac)
    {
        if (!this->is_initialized)
        {
            LOG_ERROR("Node has not been initialized");
            return false;
        }

        return memcmp(this->sta_mac_address, input_mac, 6) == 0 ||
               memcmp(this->ap_mac_address, input_mac, 6) == 0;
    }

    int EspNowNode::update()
    {
        if (!this->manager)
        {
            LOG_ERROR("Node manager has not been set");
            return ERR_NODE_MANAGER_NOT_SET;
        }

        ASSERT_OK(manager->update());
        return RESULT_OK;
    }

    int EspNowNode::read_mac_address(u8 *buffer)
    {
        memcpy(buffer, this->sta_mac_address, 6);
        return RESULT_OK;
    }

    int EspNowNode::register_peer(u8 *peer_address, esp_now_role role)
    {
        LOG_INFO("Registering new peer");

        LOG_DEBUG("Checking if peer exists: [%s]", LOG_FORMAT_MAC(peer_address));
        if (esp_now_is_peer_exist(peer_address))
        {
            LOG_WARN("Peer has already been registered: [%s]",
                     LOG_FORMAT_MAC(peer_address));
            return RESULT_DUPLICATE;
        }

        /// TODO: Add enhanced error checking (ESP32 only)
        int status = esp_now_add_peer(peer_address, role, 1, NULL, 0);
        if (status)
        {
            LOG_ERROR("Peer registration returned non zero value: [%d]", status);
            return ERR_PEER_REGISTRATION_FAILED;
        }
        LOG_INFO("Registered new peer: [%s]", LOG_FORMAT_MAC(peer_address));

        return RESULT_OK;
    }

    int EspNowNode::unregister_peer(u8 *peer_address)
    {
        LOG_INFO("Unregistering existing peer");

        LOG_DEBUG("Checking if peer exists: [%s]", LOG_FORMAT_MAC(peer_address));
        if (!esp_now_is_peer_exist(peer_address))
        {
            LOG_WARN("Peer has not been registered: [%s]",
                     LOG_FORMAT_MAC(peer_address));
            return RESULT_NO_EXIST;
        }

        /// TODO: Add enhanced error checking (ESP32 only)
        int status = esp_now_del_peer(peer_address);
        if (status)
        {
            LOG_ERROR("Peer unregistration returned non zero value: [%d]", status);
            return ERR_PEER_UNREGISTRATION_FAILED;
        }
        LOG_INFO("Unregistered existing peer: [%s]", LOG_FORMAT_MAC(peer_address));

        return RESULT_OK;
    }

    int EspNowNode::send_message(u8 *destination, MessagePayload *payload, u8 data_size)
    {

        LOG_DEBUG("Sending [%d] byte payload to [%s]", data_size,
                  LOG_FORMAT_MAC(destination));

        u8 payload_bytes[250];
        memcpy(payload_bytes, &payload->type, 1);
        memcpy(payload_bytes + 1, &payload->message_id, 2);
        memcpy(payload_bytes + 3, &payload->body, data_size);

        LOG_DEBUG("Sending:[%02x|%02x:%02x]",
                  payload_bytes[0],
                  payload_bytes[1],
                  payload_bytes[2]);

        esp_now_send(destination, (u8 *)&payload_bytes, data_size + 3);

        return RESULT_OK;
    }
}