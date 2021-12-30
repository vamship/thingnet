#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"

#include "esp_now_node.h"
#include "node_manager.h"
#include "message_handler.h"
#include "error_codes.h"

namespace thingnet
{
    const int __MAX_HANDLER_COUNT = 255;
    MessageHandler *__message_handler_list[__MAX_HANDLER_COUNT];
    MessageHandler *__default_handler = 0;
    int __message_handler_count = 0;

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

        LOG_DEBUG("Preparing message payload");
        PeerMessage message;
        memcpy(message.sender, mac_addr, 6);
        memcpy(message.payload, data, length);

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
                LOG_WARN("Error processing message by processor [%d]");
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
            return RESULT_OK;
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
            return RESULT_OK;
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
        LOG_INFO("Executing node update");

        if (!this->manager)
        {
            LOG_ERROR("Node manager has not been set");
            return ERR_NODE_MANAGER_NOT_SET;
        }

        manager->update();

        LOG_INFO("Node update completed");

        return RESULT_OK;
    }

}