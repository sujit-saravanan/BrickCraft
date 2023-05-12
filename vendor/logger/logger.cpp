#include "logger.h"
#include <iostream>
#include "rang.h"
#include <cstdarg>

void log_output(LOG_LEVEL level, const char* message, ...){
    const char* level_strings[6] = {"[FATAL]:", "[ERROR]:", "[WARN]:", "[INFO]:", "[DEBUG]:", "[TRACE]:"};
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    switch(level){
        case LOG_LEVEL_FATAL:
            std::cout << rang::bg::red << rang::style::bold << level_strings[level] << rang::style::reset << rang::bg::red << " " << out_message << rang::style::reset <<  "\n";
            exit (EXIT_FAILURE);
        case LOG_LEVEL_ERROR:
            std::cout << rang::fg::red << rang::style::bold << level_strings[level] << rang::style::reset << rang::fg::red;
            break;
        case LOG_LEVEL_WARN:
            std::cout << rang::fg::yellow << rang::style::bold << level_strings[level] << rang::style::reset << rang::fg::yellow;
            break;
        case LOG_LEVEL_TRACE:
            std::cout << rang::fg::magenta << rang::style::bold << level_strings[level] << rang::style::reset << rang::fg::magenta;
            break;
        case LOG_LEVEL_DEBUG:
            std::cout << rang::fg::blue << rang::style::bold << level_strings[level] << rang::style::reset << rang::fg::blue;
            break;
        case LOG_LEVEL_INFO:
            std::cout << rang::fg::green << rang::style::bold << level_strings[level] << rang::style::reset << rang::fg::green;
            break;
    }

    std::cout << " " << out_message << rang::style::reset <<  "\n";
}