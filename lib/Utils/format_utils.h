#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#define FORMAT_MAC(buffer, mac_addr)                                                                                                    \
    {                                                                                                                                   \
        sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]); \
    }

#endif