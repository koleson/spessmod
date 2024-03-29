#include <stdio.h>

// Define logging macros
// #define LOG_DEBUG(format, ...) printf("[D] " format "\n", ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) ;
#define LOG_INFO(format, ...)  printf("[I] " format "\n", ##__VA_ARGS__)
#define LOG_WARN(format, ...)  printf("[W] " format "\n", ##__VA_ARGS__)
#define LOG_ERROR(format, ...) printf("[E] " format "\n", ##__VA_ARGS__)
#define LOG_PACKET() printf("."); fflush(stdout);
// #define LOG_PACKET() ;

// TODO:  allow controlling log level at runtime

// TODO:  allow controlling log level