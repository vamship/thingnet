#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <espnow.h>

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

#define LOG_LEADER_FORMAT "[%3s|%12s] "
#define LOG_MESSAGE_MAX_LEN 255

namespace thingnet::utils
{
    /**
     * @brief An object that encapsulates metadata for a given module, and
     * allows the creation of log statements.
     */
    class Logger
    {
    private:
        const char *module_name;
        char buffer[LOG_MESSAGE_MAX_LEN];

    public:
        /**
         * @brief Construct a new Logger object
         * 
         * @param module_name The name of the module. This value will be
         *        included in the emitted log messages.
         */
        Logger(const char *module_name);

        /**
         * @brief Emits a log message. Prepends log level and module name to the
         * message, and allows interpolation of values.
         * 
         * @param level The log level of the message.
         * @param message The message value - can be a format string
         * @param ... 
         */
        void log(const char *level, const char *message, ...);
    };

    /**
     * @brief Utility function that formats a binary mac address into a more
     * human friendly format. This function exists for use with logging related
     * macros, and should not be used directly.
     * 
     * @param mac_addr A byte array that represents the mac address. Only the
     *        first six bytes of this array will be used.
     * @return char* A pointer to a formatted mac address string. Note that the
     * data referenced by the pointer will be overwritten on subsequent calls.
     */
    char *__log_format_mac(u8 *mac_addr);
}

#ifdef LOG_ENABLED

#define LOG_FORMAT_MAC(mac_addr) thingnet::utils::__log_format_mac(mac_addr)
#define LOG(level, message, ...)                           \
    {                                                      \
        Serial.print(level);                               \
        Serial.printf(message __VA_OPT__(, ) __VA_ARGS__); \
        Serial.println();                                  \
    }

#else

#define LOG_FORMAT_MAC(mac_addr)
#define LOG(level, message)

#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(message, ...) LOG("[ERR] ", message __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR_1(logger, message, ...) logger->log("ERR", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(message, ...) LOG("[WRN] ", message __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARN_1(logger, message, ...) logger->log("WRN", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(message, ...) LOG("[INF] ", message __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO_1(logger, message, ...) logger->log("INF", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(message, ...) LOG("[DBG] ", message __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG_1(logger, message, ...) logger->log("DBG", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#endif