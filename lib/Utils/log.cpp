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

}