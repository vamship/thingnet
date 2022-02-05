#include <Arduino.h>

#include "timer.h"
#include "debounced_input.h"

using namespace thingnet::utils;

namespace thingnet::utils
{
    DebouncedInput::DebouncedInput(uint8_t pin, u64 duration, bool trigger_low)
    {
        this->pin = pin;
        this->trigger_detected = false;
        this->debounce_timer = new Timer(duration);
        this->debounce_timer->stop();
        if (trigger_low)
        {
            this->trigger_value = LOW;
            pinMode(this->pin, INPUT_PULLUP);
        }
        else
        {
            this->trigger_value = HIGH;
            pinMode(this->pin, INPUT);
        }
    }

    DebouncedInput::DebouncedInput(uint8_t pin, u64 duration)
        : DebouncedInput(pin, duration, true)
    {
    }

    bool DebouncedInput::is_triggered()
    {
        if (!this->trigger_detected)
        {
            // We haven't sensed a trigger yet. Check to see if there's one now.
            if (digitalRead(this->pin) == this->trigger_value)
            {
                // We've detected a trigger.
                this->trigger_detected = true;
                this->debounce_timer->start();
            }

            return false;
        }

        // If we've come this far, that means a trigger was previously detected.
        if (this->debounce_timer->is_complete())
        {
            // The debounce period has completed - return true if the pin
            // reports the trigger value.
            this->trigger_detected = false;
            bool result = digitalRead(this->pin) == this->trigger_value;

            return result;
        }
        else
        {
            // Still waiting for the debounce period to expire.
            return false;
        }
    }
}