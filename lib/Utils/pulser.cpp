#include <Arduino.h>

#include "pulser.h"

namespace thingnet::utils {

    Pulser::Pulser(uint8_t pin, u64 duration)
    {
        this->is_initialized = false;
        this->pin = pin;
        this->duration = duration;
        this->duty_cycle = 50;
        this->transition_time = 0;
        this->is_high = false;
    }

    int Pulser::init()
    {
        if (this->is_initialized)
        {
            return 1;
        }
        pinMode(pin, OUTPUT);
        this->is_initialized = true;
        return 0;
    }

    int Pulser::set_duty_cycle(u8 duty_cycle)
    {
        if (duty_cycle < 0 || duty_cycle > 100)
        {
            return 1;
        }
        this->duty_cycle = duty_cycle;
        return 0;
    }

    int Pulser::set_duration(u64 duration)
    {
        this->duration = duration;
        return 0;
    }

    int Pulser::update()
    {
        if (!this->is_initialized)
        {
            return 1;
        }

        u64 delta = millis() - this->transition_time;

        // Calculate what fraction of the duration the current transition time
        // should last for.
        double limit_fraction = this->is_high ? this->duty_cycle : 100 - this->duty_cycle;
        limit_fraction = limit_fraction / 100;
        u64 transition_limit = (u64)floor(this->duration * limit_fraction);

        if (delta > transition_limit)
        {
            this->is_high = !this->is_high;
            this->transition_time = millis();
        }
        digitalWrite(this->pin, this->is_high ? HIGH : LOW);

        return 0;
    }
}