#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "esp_now_node.h"
#include "message_handler.h"
#include "log.h"
#include "format_utils.h"
#include "error_codes.h"

const int MAX_HANDLER_COUNT = 255;
MessageHandler *__message_handler_list[MAX_HANDLER_COUNT];
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
    char buffer[18];
    FORMAT_MAC(buffer, mac_addr);
    LOG_INFO("Message delivery to [%s] [%s]", buffer, status == 0 ? "ok" : "err");
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
    memcpy(&message.sender, mac_addr, 6);
    memcpy(&message.payload, data, length);

    bool processing_complete = false;
    LOG_DEBUG("Processing message");
    for (int index = 0; index < __message_handler_count; index++)
    {
        MessageHandler *processor = __message_handler_list[index];
        if (!processor->can_handle(&message))
        {
            continue;
        }

        LOG_DEBUG("Processor [%d] can handle message", index);
        ProcessingResult result = processor->process(&message);

        if (result == ProcessingResult::handled)
        {
            processing_complete = true;
            LOG_DEBUG("Processing chain completed");
            break;
        }
        else if (result == ProcessingResult::error)
        {
            processing_complete = true;
            LOG_ERROR("Error processing message");
            break;
        }
    }

    if (!processing_complete)
    {
        LOG_DEBUG("Processing chain is still not complete");
        if (__default_handler != 0)
        {
            LOG_DEBUG("Checking if default handler will process the message");
            if (__default_handler->can_handle(&message))
            {
                LOG_DEBUG("Invoking default handler");
                ProcessingResult result = __default_handler->process(&message);

                if (result == ProcessingResult::error)
                {
                    LOG_ERROR("Error executing default handler [%d]", result);
                }
            }
            else
            {
                LOG_INFO("Default handler will not handle message");
            }
        }
        else
        {
            LOG_INFO("Default handler has not been set. Skipping.");
        }
    }
}

EspNowNode::EspNowNode() {}

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
    if (this->is_initialized)
    {
        LOG_INFO("Node has already been initialized");
        return RESULT_OK;
    }

    LOG_DEBUG("Reading current mac address");
    WiFi.macAddress(this->mac_address);

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


    char mac_addr_str[18];
    FORMAT_MAC(mac_addr_str, this->mac_address);
    LOG_DEBUG("Node initialized: [%s]", mac_addr_str);

    return RESULT_OK;
}

int EspNowNode::add_handler(MessageHandler *handler)
{
    if (!this->is_initialized)
    {
        LOG_ERROR("Node has not been initialized");
        return ERR_NODE_NOT_INITIALIZED;
    }

    if (__message_handler_count >= MAX_HANDLER_COUNT)
    {
        LOG_ERROR("Cannot add handler - maximum handler limit has been reached");
        return ERR_HANDLER_LIMIT_EXCEEDED;
    }

    LOG_INFO("Adding new handler to list");

    // Add handler to the end of the list.
    __message_handler_list[__message_handler_count] = handler;
    __message_handler_count++;

    LOG_INFO("Handler added successfully. Total handlers: [%d]", __message_handler_count);

    return RESULT_OK;
}

int EspNowNode::set_default_handler(MessageHandler *handler)
{
    if (!this->is_initialized)
    {
        LOG_ERROR("Node has not been initialized");
        return ERR_NODE_NOT_INITIALIZED;
    }

    LOG_INFO("Setting default handler");

    // Set default handler
    __default_handler = handler;

    LOG_INFO("Default handler has been set");

    return RESULT_OK;
}

bool EspNowNode::has_mac_address(u8 *input_mac)
{
    if (!this->is_initialized)
    {
        LOG_ERROR("Node has not been initialized");
        return false;
    }

    return memcmp(this->mac_address, input_mac, 6) == 0;
}