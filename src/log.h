#include <Arduino.h>
#include <espnow.h>

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#ifdef LOG_ENABLED
    // #define LOG(level, message) Serial.println("[" + level + "] " + message);
    #define LOG(level, message) {\
    Serial.print("[");\
    Serial.print(level);\
    Serial.print("] ");\
    Serial.println(message); \
}

#else
    #define LOG(level, message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(message) LOG("ERROR", message)
#else
    #define LOG_ERROR(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(message) LOG("INFO ", message)
#else
    #define LOG_INFO(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(message) LOG("DEBUG", message)
#else
    #define LOG_DEBUG(message)
#endif