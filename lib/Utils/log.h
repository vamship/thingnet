#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <espnow.h>

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#ifdef LOG_ENABLED
#define LOG(level, message, ...)                           \
    {                                                      \
        Serial.print(level);                               \
        Serial.printf(message __VA_OPT__(, ) __VA_ARGS__); \
        Serial.println();                                  \
    }
#else
#define LOG(level, message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(message, ...) LOG("[ERROR] ", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(message, ...) LOG("[INFO ] ", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(message, ...) LOG("[DEBUG] ", message __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#endif