#include <Arduino.h>

#include "log.h"
#include "error_codes.h"

#include "hadrware_manager.h"

using namespace thingnet;
using namespace thingnet::utils;

static Logger *logger = new Logger("hw-mgr");

HardwareManager::HardwareManager()
{
    this->is_initialized = false;
    this->server_mode_pin_value = LOW;
    this->gpio_in_server_mode = 14;
    this->gpio_in_advertise = 12;
}

int HardwareManager::initialize()
{
    if (this->is_initialized)
    {
        LOG_WARN(logger, "Hardware manager has already been initialized");
        return thingnet::RESULT_DUPLICATE;
    }

    LOG_TRACE(logger, "Configuring GPIO pins");
    pinMode(this->gpio_in_server_mode, INPUT_PULLUP);
    pinMode(this->gpio_in_advertise, INPUT_PULLUP);

    LOG_DEBUG(logger, "Reading inital pin values");
    this->server_mode_pin_value = digitalRead(this->gpio_in_server_mode);

    LOG_TRACE(logger, "Setting initialization flag");
    this->is_initialized = true;

    LOG_INFO(logger, "Hardware manager initialized");
    return thingnet::RESULT_OK;
}

int HardwareManager::update()
{
    if (!this->is_initialized)
    {
        LOG_ERROR(logger, "Hardware manager has not been initialized");
        return HW_MGR_ERR_NOT_INITIALIZED;
    }

    this->advertise_pin_value = digitalRead(this->gpio_in_advertise);

    return thingnet::RESULT_OK;
}

bool HardwareManager::is_server_mode()
{
    return this->server_mode_pin_value == LOW;
}

bool HardwareManager::send_advertisement()
{
    return this->is_server_mode() && this->advertise_pin_value == LOW;
}