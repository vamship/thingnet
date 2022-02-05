#ifndef __DEBOUNCED_INPUT_H
#define __DEBOUNCED_INPUT_H

#include <Arduino.h>
#include "timer.h"

namespace thingnet::utils
{
    /**
     * @brief Abstracted class that can detect a detect a digital high/low after
     * debouncing for a preset duration.
     */
    class DebouncedInput
    {
    private:
        uint8_t pin;
        uint8_t trigger_value;
        bool trigger_detected;
        Timer *debounce_timer;

    public:
        /**
         * @brief Construct a new Debounced Input object
         *
         * @param pin The GPIO pin number to use to capture the input.
         * @param duration The debounce duration in milliseconds
         * @param trigger_low If set to true, detects a trigger if the input
         *        is low.
         */
        DebouncedInput(uint8_t pin, u64 duration, bool trigger_low);

        /**
         * @brief Construct a new Debounced Input object, that defaults the
         * trigger value to low.
         *
         * @param pin The GPIO pin number to use to capture the input.
         * @param duration The debounce duration in milliseconds
         */
        DebouncedInput(uint8_t pin, u64 duration);

        /**
         * @brief Detects whether or not the input has been triggered after
         * the specified debounce period.
         *
         * @return true The input has been triggered
         * @return false The input has not been triggered
         */
        bool is_triggered();
    };
}

#endif