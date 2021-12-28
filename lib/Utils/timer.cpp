#include <Arduino.h>
#include "timer.h"

Timer::Timer(u64 duration, bool auto_restart)
{
    this->duration = duration;
    this->auto_restart = auto_restart;
    this->last_time = 0;
}

Timer::Timer(u64 duration) : Timer(duration, false){};

bool Timer::is_complete()
{
    if (this->is_started && (millis() - this->last_time > this->duration))
    {
        this->is_started = false;
        if (this->auto_restart)
        {
            this->start();
        }
        return true;
    }
    return false;
}

int Timer::start()
{
    if (this->is_started)
    {
        return 1;
    }
    this->last_time = millis();
    this->is_started = true;

    return 0;
}

int Timer::stop()
{
    if (!this->is_started)
    {
        return 1;
    }
    this->is_started = false;

    return 0;
}

int Timer::restart()
{
    this->stop();
    return this->start();

    return 0;
}