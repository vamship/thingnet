#ifdef ESP32
#include <Wifi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <espnow.h>

#include "log.h"

#include "messages.h"
#include "node.h"
#include "node_profile.h"
#include "message_handler.h"
#include "error_codes.h"

static Logger *logger = new Logger("node");

namespace thingnet
{
    static const u8 __MAX_HANDLER_COUNT = 255;
    static MessageHandler *__message_handler_list[__MAX_HANDLER_COUNT];
    static MessageHandler *__default_handler = 0;
    static u8 __message_handler_count = 0;

    /**
     * @brief Handles data send confirmation.
     *
     * @param mac_addr The mac address of the recipient
     * @param status Whether or not the message was sent successfully
     */
    void __on_data_sent(u8 *mac_addr, u8 status)
    {
        LOG_TRACE(logger, "Message delivery ack to [%s] [%s]", LOG_FORMAT_MAC(mac_addr), status == 0 ? "ok" : "err");
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
        LOG_TRACE(logger, "Processing message from peer");
        LOG_DEBUG(logger, "Received [%02x|%02x:%02x] + [%d] bytes from [%s]",
                  data[0],
                  data[1],
                  data[2],
                  length - 3,
                  LOG_FORMAT_MAC(mac_addr));

        PeerMessage message;
        memcpy(message.sender, mac_addr, 6);
        memcpy(&message.payload.type, data, 1);
        memcpy(&message.payload.message_id, data + 1, 2);
        memcpy(&message.payload.body, data + 3, length - 3);

        bool processing_complete = false;
        LOG_TRACE(logger, "Starting handler chain");
        for (int index = 0; index < __message_handler_count; index++)
        {
            MessageHandler *handler = __message_handler_list[index];
            if (!handler->can_handle(&message))
            {
                LOG_TRACE(logger, "Handler [%d] will not handle message", index);
                continue;
            }

            LOG_DEBUG(logger, "Handler [%d] will handle message", index);
            ProcessingResult result = handler->process(&message);

            if (result == ProcessingResult::handled)
            {
                processing_complete = true;
                LOG_TRACE(logger, "Handler chain completed");
                break;
            }
            else if (result == ProcessingResult::error)
            {
                processing_complete = true;
                LOG_WARN(logger, "Error processing message by processor [%d]", index);
                break;
            }
        }

        if (!processing_complete)
        {
            LOG_TRACE(logger, "Handler chain is still not complete");
            if (__default_handler != 0)
            {
                LOG_TRACE(logger, "Checking if default handler will process the message");
                if (__default_handler->can_handle(&message))
                {
                    LOG_DEBUG(logger, "Invoking default handler");
                    ProcessingResult result = __default_handler->process(&message);

                    if (result == ProcessingResult::error)
                    {
                        LOG_WARN(logger, "Error processing message by default handler");
                    }
                }
                else
                {
                    LOG_WARN(logger, "Default handler will not handle message");
                }
            }
            else
            {
                LOG_WARN(logger, "Default handler has not been set. Skipping.");
            }
        }

        LOG_TRACE(logger, "Message from peer processed");
    }

    Node::Node()
    {
        this->profile = 0;
    }

    Node::~Node()
    {
        // Loop through all handlers and delete them.
        for (int index = 0; index < __message_handler_count; index++)
        {
            delete __message_handler_list[index];
        }
        __message_handler_count = 0;
    }

    Node &Node::get_instance()
    {
        static Node instance;
        return instance;
    }

    int Node::init()
    {
        LOG_TRACE(logger, "Initializing node");
        if (this->is_initialized)
        {
            LOG_WARN(logger, "Node has already been initialized");
            return RESULT_DUPLICATE;
        }

        LOG_TRACE(logger, "Reading current mac address(es)");
        WiFi.macAddress(this->sta_mac_address);
        WiFi.softAPmacAddress(this->ap_mac_address);

        LOG_DEBUG(logger, "Setting wifi to station mode");
        WiFi.mode(WIFI_AP_STA);

        LOG_TRACE(logger, "Initializing ESP-NOW");
        if (esp_now_init() != 0)
        {
            LOG_ERROR(logger, "Error initializing ESP-NOW");
            return ERR_ESP_NOW_INIT_FAILED;
        }
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

        LOG_TRACE(logger, "Registering send/receive callbacks");
        esp_now_register_send_cb(__on_data_sent);
        esp_now_register_recv_cb(__on_data_received);

        LOG_TRACE(logger, "Initializing node profile");
        if (this->profile == 0)
        {
            LOG_ERROR(logger, "Node profile not set");
            return ERR_NODE_PROFILE_NOT_SET;
        }
        ASSERT_OK(this->profile->init());

        this->is_initialized = true;

        LOG_DEBUG(logger, "AP MAC Address : [%s]", LOG_FORMAT_MAC(this->ap_mac_address));
        LOG_DEBUG(logger, "STA MAC Address: [%s]", LOG_FORMAT_MAC(this->sta_mac_address));

        LOG_TRACE(logger, "Node initialized");

        return RESULT_OK;
    }

    u16 Node::get_next_message_id()
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node has not been initialized");
            return 0;
        }
        this->message_id++;
        return this->message_id;
    }

    NodeProfile *Node::get_profile()
    {
        if (!this->is_initialized)
        {
            LOG_WARN(logger, "Node has not been initialized");
            return 0;
        }
        return this->profile;
    }

    int Node::add_handler(MessageHandler *handler)
    {
        LOG_TRACE(logger, "Registering message handler");

        if (!this->is_initialized)
        {
            LOG_ERROR(logger, "Node has not been initialized");
            return ERR_NODE_NOT_INITIALIZED;
        }

        if (__message_handler_count >= __MAX_HANDLER_COUNT)
        {
            LOG_ERROR(logger, "Cannot add handler - maximum handler limit has been reached");
            return ERR_HANDLER_LIMIT_EXCEEDED;
        }

        // Add handler to the end of the list.
        __message_handler_list[__message_handler_count] = handler;
        __message_handler_count++;

        LOG_DEBUG(logger, "Handler added successfully. Total handlers: [%d]", __message_handler_count);

        return RESULT_OK;
    }

    int Node::remove_handler(MessageHandler *handler)
    {
        LOG_TRACE(logger, "Unregistering message handler");

        if (!this->is_initialized)
        {
            LOG_ERROR(logger, "Node has not been initialized");
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
            LOG_WARN(logger, "Could not find message handler");
            return RESULT_NO_EXIST;
        }
        __message_handler_count -= find_count;

        LOG_TRACE(logger, "Handler(s) removed successfully [%d]. Total handlers: [%d]",
                  find_count,
                  __message_handler_count);

        return RESULT_OK;
    }

    int Node::set_node_profile(NodeProfile *profile)
    {
        LOG_TRACE(logger, "Registering node profile");

        if (this->is_initialized)
        {
            LOG_ERROR(logger, "Node has already been initialized");
            return RESULT_DUPLICATE;
        }

        if (this->profile != 0)
        {
            LOG_WARN(logger, "Node profile has already been registered");
            return RESULT_DUPLICATE;
        }

        this->profile = profile;

        LOG_TRACE(logger, "Configuring default handler");
        __default_handler = profile;

        LOG_TRACE(logger, "Node profile registered");

        return RESULT_OK;
    }

    bool Node::has_mac_address(u8 *input_mac)
    {
        if (!this->is_initialized)
        {
            LOG_ERROR(logger, "Node has not been initialized");
            return false;
        }

        return memcmp(this->sta_mac_address, input_mac, 6) == 0 ||
               memcmp(this->ap_mac_address, input_mac, 6) == 0;
    }

    int Node::update()
    {
        if (!this->profile)
        {
            LOG_ERROR(logger, "Node profile has not been set");
            return ERR_NODE_PROFILE_NOT_SET;
        }

        ASSERT_OK(profile->update());
        return RESULT_OK;
    }

    int Node::read_mac_address(u8 *buffer)
    {
        memcpy(buffer, this->ap_mac_address, 6);
        return RESULT_OK;
    }

    int Node::register_peer(u8 *peer_address, esp_now_role role)
    {
        LOG_TRACE(logger, "Registering new peer");

        LOG_TRACE(logger, "Checking if peer exists: [%s]", LOG_FORMAT_MAC(peer_address));
        if (esp_now_is_peer_exist(peer_address))
        {
            LOG_WARN(logger, "Peer has already been registered: [%s]",
                     LOG_FORMAT_MAC(peer_address));
            return RESULT_DUPLICATE;
        }

        /// TODO: Add enhanced error checking (ESP32 only)
        int status = esp_now_add_peer(peer_address, role, 1, NULL, 0);
        if (status)
        {
            LOG_ERROR(logger, "Peer registration returned non zero value: [%d]", status);
            return ERR_PEER_REGISTRATION_FAILED;
        }
        LOG_DEBUG(logger, "Registered new peer: [%s]", LOG_FORMAT_MAC(peer_address));

        return RESULT_OK;
    }

    int Node::unregister_peer(u8 *peer_address)
    {
        LOG_TRACE(logger, "Unregistering existing peer");

        LOG_TRACE(logger, "Checking if peer exists: [%s]", LOG_FORMAT_MAC(peer_address));
        if (!esp_now_is_peer_exist(peer_address))
        {
            LOG_WARN(logger, "Peer has not been registered: [%s]",
                     LOG_FORMAT_MAC(peer_address));
            return RESULT_NO_EXIST;
        }

        /// TODO: Add enhanced error checking (ESP32 only)
        int status = esp_now_del_peer(peer_address);
        if (status)
        {
            LOG_ERROR(logger, "Peer unregistration returned non zero value: [%d]", status);
            return ERR_PEER_UNREGISTRATION_FAILED;
        }
        LOG_DEBUG(logger, "Unregistered existing peer: [%s]", LOG_FORMAT_MAC(peer_address));

        return RESULT_OK;
    }

    int Node::send_message(u8 *destination, MessagePayload *payload, u8 data_size)
    {
        u8 payload_bytes[250];
        u16 message_id = payload->message_id == 0 ? this->get_next_message_id()
                                                  : payload->message_id;

        memcpy(payload_bytes, &payload->type, 1);
        memcpy(payload_bytes + 1, &message_id, 2);
        memcpy(payload_bytes + 3, &payload->body, data_size);

        LOG_DEBUG(logger, "Sending [%02x|%02x:%02x] + [%d] bytes to [%s]",
                  payload_bytes[0],
                  payload_bytes[1],
                  payload_bytes[2],
                  data_size,
                  LOG_FORMAT_MAC(destination));

        esp_now_send(destination, (u8 *)&payload_bytes, data_size + 3);

        return RESULT_OK;
    }
}