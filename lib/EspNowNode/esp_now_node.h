#include "Arduino.h"
#include <espnow.h>

/**
 * @brief Represents a node that can communicate using ESP-NOW
 */
class EspNowNode
{
private:
    String mac_address;
    bool is_initialized;
    EspNowNode();

public:
    /**
     * @brief Get the instance object
     *
     * @return EspNowNode&
     */
    static EspNowNode &get_instance();

    /**
     * @brief Initializes the node and returns a non-zero value if the
     * initialization fails.
     *
     * @return int
     */
    int init();

    // Singleton implementation.
    // See: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
    EspNowNode(EspNowNode const &) = delete;
    void operator=(EspNowNode const &) = delete;
};