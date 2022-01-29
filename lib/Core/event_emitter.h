#ifndef __EVENT_EMITTER_H
#define __EVENT_EMITTER_H

#include <Arduino.h>

namespace thingnet
{
    const int __MAX_LISTENER_COUNT = 32;

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
}

#endif