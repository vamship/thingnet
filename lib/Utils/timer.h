#ifndef __TIMER_H
#define __TIMER_H

#include <Arduino.h>

/**
 * @brief Simple timer implementation that determines if a certain amount of
 * time has elapsed since it was checked previously. The checks only take place
 * when the is_complete() method is called.
 */
class Timer
{
private:
    u64 duration;
    u64 last_time;
    bool is_started;
public:
    /**
     * @brief Construct a new Timer object
     * 
     * @param duration The duration of the timer.
     */
    Timer(u64 duration);

    /**
     * @brief Determines whether or not the timer is complete. If complete, a
     * new timer will be started from the current time.
     * 
     * @return true If the timer has completed
     * @return false If the timer has not completed, or has been stopped.
     */
    bool is_complete();

    /**
     * @brief Starts the timer. If the timer has been started, this method will
     * have no effect.
     * 
     * @return int A non zero value will be returned if the timer cannot be
     *         started.
     */
    int start();

    /**
     * @brief Starts the timer.
     * 
     * @return int A non zero value will be returned if the timer cannot be
     *         started.
     */
    int stop();

    /**
     * @brief Restarts the timer - if the timer has been stopped, it will be
     * started, and if it has been stopped, it will be started again.
     * 
     * @return int 
     */
    int restart();
};

#endif