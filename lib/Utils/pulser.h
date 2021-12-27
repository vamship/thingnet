#ifndef __PULSER_H
#define __PULSER_H

#include <Arduino.h>

/**
 * @brief A class that can be used to generate a periodic pulse without blocking
 * the event loop.
 */
class Pulser
{
private:
    uint8_t pin;
    u64 duration;
    u64 transition_time;
    u8 duty_cycle;
    bool is_high;
    bool is_initialized;

public:
    /**
     * @brief Construct a new Pulser object
     * 
     * @param pin The GPIO pin on which the pulses will be generated
     * @param duration The duration of the pulse. The pulser will be initialized
     *        with a duty cycle of 0.5.
     */
    Pulser(uint8_t pin, u64 duration);

    /**
     * @brief Initializes the pulser object
     * 
     * @return int A non zero value will be returned if the initialization
     *         fails.
     */
    int init();

    /**
     * @brief Set the duty cycle of the output pulses.
     * 
     * @param duty_cycle The duty cycle as a number from 1 to 100
     * @return int A non zero value will be returned if the duty cycle could not
     *         be set correctly.
     */
    int set_duty_cycle(u8 duty_cycle);

    /**
     * @brief Set the duration of the output pulses
     * 
     * @param duration The duration in milliseconds.
     * @return int A non zero value will be returned if the duration could not
     *         be set correctly.
     */
    int set_duration(u64 duration);

    /**
     * @brief Allows the pulser to update its state. This is a non blocking call
     * that will transition the state of the pulse (high -> low or low -> high)
     * based on the current time and the duty cycle of the pulser.
     * 
     * @return int A non zero value will be returned if the update fails.
     */
    int update();
};

#endif