#ifndef __EVENT_EMITTER_H
#define __EVENT_EMITTER_H

#include <Arduino.h>
#include "log.h"
#include "error_codes.h"

using namespace thingnet::utils;

namespace thingnet
{
    const int __MAX_LISTENER_COUNT = 32;
    static Logger *_event_emitter_logger = new Logger("event-emit");

    /**
     * @brief A generic event base class that can accept and manage a collection
     * of listeners.
     * 
     * @tparam T The data type of the event payload.
     */
    template <typename T>
    class Event
    {
    protected:
        void (*listeners[__MAX_LISTENER_COUNT])(int event_type, T event_data);
        u16 listener_count;

    public:
        /**
         * @brief Construct a new event object
         */
        Event();

        /**
         * @brief Destroy the Event object
         */
        virtual ~Event();

        /**
         * @brief Registers a listener. This method will do nothing if the
         * listener has already been registered. Listeners will be notified of
         * the event in an indeterminate order. It is strongly recommended that
         * each listener completes execution quickly, allowing other listeners
         * to be invoked.
         * 
         * @param listener Pointer to a function that will be invoked when the
         *        event occurs.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int add_listener(void (*listener)(int event_type, T event_data));

        /**
         * @brief Removes an existing listener from the list. This method will
         * do nothing if the listener had previously not been registered.
         * 
         * @param listener Pointer to the listener function to be removed.
         * 
         * @return int A non success value will be returned if the add operation
         *         resulted in an error. See error codes for more information.
         */
        int remove_listener(void (*listener)(int event_type, T event_data));
    };

    /**
     * @brief Allows events to be triggered on an Event class. This is
     * typically used to hide the notification function from listeners when
     * passing references downstream. The owner class should typecast the
     * EventEmitter as an Event reference, allowing consumers to register
     * listeners but not actually trigger notifications.
     * 
     * @tparam T The data type of the event payload.
     */
    template <typename T>
    class EventEmitter : public Event<T>
    {
    private:
        int event_type;

    public:
        /**
         * @brief Construct a new event emitter object
         * 
         * @param event_type The type of event that this event emitter is
         *        defined for
         */
        EventEmitter(int event_type);

        /**
         * @brief Destroy the event emitter object
         */
        virtual ~EventEmitter();

        /**
         * @brief Notify all registered listeners of an event.
         * 
         * @param event_data The event data to be passed to each listener.
         */
        void emit(T event_data);
    };

    template <typename T>
    Event<T>::Event()
    {
        this->listener_count = 0;
    }

    template <typename T>
    Event<T>::~Event()
    {
        // Nothing to do here.
    }

    template <typename T>
    int Event<T>::add_listener(void (*listener)(int event_type, T event_data))
    {
        LOG_TRACE(_event_emitter_logger, "Registering event listener");
        for (int index = 0; index < this->listener_count; index++)
        {
            if (this->listeners[index] == listener)
            {
                LOG_WARN(_event_emitter_logger, "Listener has already been registered");
                return RESULT_DUPLICATE;
            }
        }
        this->listeners[this->listener_count] = listener;
        this->listener_count++;
        LOG_DEBUG(_event_emitter_logger, "Listener registered. Total listeners [%d]",
                  this->listener_count);
        return RESULT_OK;
    }

    template <typename T>
    int Event<T>::remove_listener(void (*listener)(int event_type, T event_data))
    {
        LOG_TRACE(_event_emitter_logger, "Removing event listener");
        u16 listener_index = this->listener_count;
        for (u16 index = 0; index < this->listener_count; index++)
        {
            if (this->listeners[index] == listener)
            {
                listener_index = index;
                break;
            }
        }

        if (listener_index == this->listener_count)
        {
            LOG_WARN(_event_emitter_logger, "Cannot remove unregistered listener");
            return RESULT_NO_EXIST;
        }

        LOG_TRACE(_event_emitter_logger, "Found listener at [%d]", listener_index);
        for (u16 index = listener_index;
             listener_index < this->listener_count - 1;
             listener_index++)
        {
            this->listeners[index] = this->listeners[index + 1];
        }

        this->listener_count--;
        this->listeners[this->listener_count] = 0;
        LOG_DEBUG(_event_emitter_logger, "Listener removed. Total listeners [%d]",
                  this->listener_count);

        return RESULT_OK;
    }

    template <typename T>
    EventEmitter<T>::EventEmitter(int event_type) : Event<T>()
    {
        this->event_type = event_type;
    }

    template <typename T>
    EventEmitter<T>::~EventEmitter()
    {
        // Nothing to do here.
    }

    template <typename T>
    void EventEmitter<T>::emit(T event_data)
    {
        LOG_DEBUG(_event_emitter_logger, "Emitting event [%d]. Listener count [%d]",
                  this->event_type, this->listener_count);
        for (u16 index = 0; index < this->listener_count; index++)
        {
            LOG_TRACE(_event_emitter_logger, "Notifying listener [%d]");
            this->listeners[index](this->event_type, event_data);
        }

        LOG_TRACE(_event_emitter_logger, "Notified [%d] listeners", this->listener_count);
    }
}

#endif