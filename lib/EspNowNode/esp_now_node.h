#ifndef __ESP_NOW_NODE_H
#define __ESP_NOW_NODE_H

#include <Arduino.h>
#include <espnow.h>
#include "message_handler.h"

/**
 * @brief Represents a node that can communicate using ESP-NOW
 */
class EspNowNode
{
private:
    String mac_address;
    bool is_initialized;
    EspNowNode();
    ~EspNowNode();

public:
    /**
     * @brief Get the instance object
     *
     * @return EspNowNode&
     */
    static EspNowNode &get_instance();

    /**
     * @brief Initializes the node and returns a non-zero value if the
     * initialization fails.
     *
     * @return int
     */
    int init();

    /**
     * @brief Adds a message handler to the end of the list of available
     * handlers for the node. 
     * 
     * @param handler The handler to add
     * @return int A non zero value will be returned if the add operation
     *         resulted in an error.
     */
    int add_handler(MessageHandler *handler);

    // Singleton implementation.
    // See: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
    EspNowNode(EspNowNode const &) = delete;
    void operator=(EspNowNode const &) = delete;
};

#endif
