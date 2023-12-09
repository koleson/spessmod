#include <stdio.h>

// Define logging macros
#define LOG_DEBUG(format, ...) printf("[D] " format "\n", ##__VA_ARGS__)
#define LOG_INFO(format, ...)  printf("[I] " format "\n", ##__VA_ARGS__)
#define LOG_WARN(format, ...)  printf("[W] " format "\n", ##__VA_ARGS__)
#define LOG_ERROR(format, ...) printf("[E] " format "\n", ##__VA_ARGS__)

// TODO:  allow controlling log level