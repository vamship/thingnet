#include <Arduino.h>

#include "log.h"
#include "error_codes.h"
#include "debounced_input.h"

#include "hadrware_manager.h"

using namespace thingnet;
using namespace thingnet::utils;

static Logger *logger = new Logger("hw-mgr");

HardwareManager::HardwareManager()
{
    this->is_initialized = false;
    this->is_server_mode_enabled = false;
    this->is_advertise_triggered = false;
}

int HardwareManager::initialize()
{
    if (this->is_initialized)
    {
        LOG_WARN(logger, "Hardware manager has already been initialized");
        return thingnet::RESULT_DUPLICATE;
    }

    LOG_TRACE(logger, "Configuring GPIO pins");
    this->server_mode_input = new DebouncedInput(14, 100);
    this->advertise_input = new DebouncedInput(12, 3000);

    LOG_DEBUG(logger, "Reading inital pin values");
    this->server_mode_input->is_triggered();
    // Sleep for a bit to allow the debounce checker to work.
    delay(105);
    this->is_server_mode_enabled = this->server_mode_input->is_triggered();

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

    this->is_advertise_triggered = this->advertise_input->is_triggered();

    return thingnet::RESULT_OK;
}

bool HardwareManager::is_server_mode()
{
    return this->is_server_mode_enabled;
}

bool HardwareManager::send_advertisement()
{
    return this->is_server_mode() && this->is_advertise_triggered;
}