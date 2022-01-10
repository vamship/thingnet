#include "Arduino.h"
#include "log.h"

namespace thingnet::utils
{
    char __mac_str[18];

    char *__log_format_mac(u8 *mac_addr)
    {
        sprintf(__mac_str,
                "%02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0],
                mac_addr[1],
                mac_addr[2],
                mac_addr[3],
                mac_addr[4],
                mac_addr[5]);
        return __mac_str;
    }

    Logger::Logger(const char *module_name)
    {
        this->module_name = module_name;
    }

    void Logger::log(const char *level, const char *message, ...)
    {
        va_list v_args;
        va_start(v_args, message);
        vsprintf(this->buffer, message, v_args);
        va_end(v_args);

        char leader[32];
        sprintf(leader, LOG_LEADER_FORMAT, level, this->module_name);

        Serial.print(leader);
        Serial.println(this->buffer);
    }
}