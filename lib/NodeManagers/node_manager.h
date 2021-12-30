#ifndef __NODE_MANAGER_H
#define __NODE_MANAGER_H

#include <Arduino.h>
#include "message_handler.h"

using namespace thingnet::message_handlers;

namespace thingnet::node_managers
{
    /**
     * @brief Base class for node manager entities. Node managers are
     * designed to manage the basic operation of the node - adding peers,
     * updating them at regular intervals, etc.
     * 
     * Node managers should be designed to function as the default message
     * handlers for a node, dealing with messages that are not handled by
     * upstream handlers.
     * 
     * This class can be extended and customized for specific node profiles,
     * such as servers, clients, etc.
     */
    class NodeManager : public MessageHandler
    {
    public:
        /**
         * @brief Construct a new Node Manager object
         */
        NodeManager();

        /**
         * @brief Destroy the Node Manager object. This is a virtual method
         * that should be implemented by child classes as appropriate.
         */
        virtual ~NodeManager();

        /**
         * @brief Allows the node manager to initialize itself. This method
         * will be called once at the start of the program.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int init();

        /**
         * @brief Allows the node manager to update itself. This method will
         * typically be called from within a processing loop, and must be
         * non blocking.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        virtual int update() = 0;
    };
}

#endif