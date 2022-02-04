#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>

const int HW_MGR_ERR_NOT_INITIALIZED = 0x70;

class HardwareManager
{
private:
    uint8_t gpio_in_server_mode;
    uint8_t gpio_in_advertise;

    uint8_t server_mode_pin_value;
    uint8_t advertise_pin_value;
    bool is_initialized;

public:
    /**
     * @brief Construct a new Hardware Manager object
     */
    HardwareManager();

    /**
     * @brief Allows the hardware manager to initialize itself. This method is
     * typically invoked once at the start of the program.
     *
     * @return int A non success value will be returned if the add operation
     *         resulted in an error. See error codes for more information.
     */
    int initialize();

    /**
     * @brief Allows the hardware manager to update itself. This will allow the
     * manager to read inputs and set output pins based on its state.
     *
     * @return int A non success value will be returned if the add operation
     *         resulted in an error. See error codes for more information.
     */
    int update();

    /**
     * @brief Determines if the device is currently in server mode.
     *
     * @return true The device is in server mode
     * @return false The device is not in server mode
     */
    bool is_server_mode();

    /**
     * @brief Determines whether or not to send an advertisement message. This
     * only applies when the node is in server mode.
     * 
     * @return true Send an advertisement
     * @return false Do not advertise
     */
    bool send_advertisement();
};

#endif