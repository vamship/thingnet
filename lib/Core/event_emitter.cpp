#include <Arduino.h>
#include "log.h"
#include "timer.h"

#include "error_codes.h"
#include "event_emitter.h"

using namespace thingnet::utils;

static Logger *logger = new Logger("event-emit");

namespace thingnet
{
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
        LOG_TRACE(logger, "Registering event listener");
        for (int index = 0; index < this->listener_count; index++)
        {
            if (this->listeners[index] == listener)
            {
                LOG_WARN(logger, "Listener has already been registered");
                return RESULT_DUPLICATE;
            }
        }
        this->listeners[this->listener_count] = listener;
        this->listener_count++;
        LOG_DEBUG(logger, "Listener registered. Total listeners [%d]",
                  this->listener_count);
        return RESULT_OK;
    }

    template <typename T>
    int Event<T>::remove_listener(void (*listener)(int event_type, T event_data))
    {
        LOG_TRACE(logger, "Removing event listener");
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
            LOG_WARN(logger, "Cannot remove unregistered listener");
            return RESULT_NO_EXIST;
        }

        LOG_TRACE(logger, "Found listener at [%d]", listener_index);
        for (u16 index = listener_index;
             listener_index < this->listener_count - 1;
             listener_index++)
        {
            this->listeners[index] = this->listeners[index + 1];
        }

        this->listener_count--;
        this->listeners[this->listener_count] = 0;
        LOG_DEBUG(logger, "Listener removed. Total listeners [%d]",
                  this->listener_count);

        return RESULT_OK;
    }

    template <typename T>
    EventEmitter<T>::EventEmitter(int event_type)
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
        LOG_DEBUG(logger, "Emitting event [%d]. Listener count [%d]",
                  event_data, this->listener_count);
        for (u16 index = 0; index < this->listener_count; index++)
        {
            LOG_TRACE(logger, "Notifying listener [%d]");
            this->listeners[index](event_data);
        }

        LOG_TRACE(logger, "Notified [%d] listeners", this->listener_count);
    }
}