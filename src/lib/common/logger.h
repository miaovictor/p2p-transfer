//
// Created by victor on 19-3-5.
//

#ifndef SRC_LIB_COMMON_LOGGER_H
#define SRC_LIB_COMMON_LOGGER_H

#include "log4cplus/loggingmacros.h"

namespace vzc {

extern log4cplus::Logger logger;

bool InitializeLogger(bool tofile = false, const std::string &file_path = "");

void UnInitializeLogger();

}


#ifndef LOG_TRACE
#define LOG_TRACE(logEvent) LOG4CPLUS_TRACE(logger, logEvent)
#endif //LOG_TRACE
#ifndef LOG_TRACE_FMT
#define LOG_TRACE_FMT(...) LOG4CPLUS_TRACE_FMT(logger, __VA_ARGS__)
#endif //LOG_TRACE_FMT
#ifndef LOG_TRACE_STR
#define LOG_TRACE_STR(logEvent) LOG4CPLUS_TRACE_STR(logger, logEvent)
#endif //LOG_TRACE_STR

#ifndef LOG_DEBUG
#define LOG_DEBUG(logEvent) LOG4CPLUS_DEBUG(logger, logEvent)
#endif //LOG_DEBUG
#ifndef LOG_DEBUG_FMT
#define LOG_DEBUG_FMT(...) LOG4CPLUS_DEBUG_FMT(logger, __VA_ARGS__)
#endif //LOG_DEBUG_FMT
#ifndef LOG_DEBUG_STR
#define LOG_DEBUG_STR(logEvent) LOG4CPLUS_DEBUG_STR(logger, logEvent)
#endif //LOG_DEBUG_STR

#ifndef LOG_INFO
#define LOG_INFO(logEvent) LOG4CPLUS_INFO(logger, logEvent)
#endif //LOG_INFO
#ifndef LOG_INFO_FMT
#define LOG_INFO_FMT(...) LOG4CPLUS_INFO_FMT(logger, __VA_ARGS__)
#endif //LOG_INFO_FMT
#ifndef LOG_INFO_STR
#define LOG_INFO_STR(logEvent) LOG4CPLUS_INFO_STR(logger, logEvent)
#endif //LOG_INFO_STR

#ifndef LOG_WARN
#define LOG_WARN(logEvent) LOG4CPLUS_WARN(logger, logEvent)
#endif //LOG_WARN
#ifndef LOG_WARN_FMT
#define LOG_WARN_FMT(...) LOG4CPLUS_WARN_FMT(logger, __VA_ARGS__)
#endif //LOG_WARN_FMT
#ifndef LOG_WARN_STR
#define LOG_WARN_STR(logEvent) LOG4CPLUS_WARN_STR(logger, logEvent)
#endif //LOG_WARN_STR

#ifndef LOG_ERROR
#define LOG_ERROR(logEvent) LOG4CPLUS_ERROR(logger, logEvent)
#endif //LOG_ERROR
#ifndef LOG_ERROR_FMT
#define LOG_ERROR_FMT(...) LOG4CPLUS_ERROR_FMT(logger, __VA_ARGS__)
#endif //LOG_ERROR_FMT
#ifndef LOG_ERROR_STR
#define LOG_ERROR_STR(logEvent) LOG4CPLUS_ERROR_STR(logger, logEvent)
#endif //LOG_ERROR_STR

#ifndef LOG_FATAL
#define LOG_FATAL(logEvent) LOG4CPLUS_FATAL(logger, logEvent)
#endif //LOG_FATAL
#ifndef LOG_FATAL_FMT
#define LOG_FATAL_FMT(...) LOG4CPLUS_FATAL_FMT(logger, __VA_ARGS__)
#endif //LOG_FATAL_FMT
#ifndef LOG_FATAL_STR
#define LOG_FATAL_STR(logEvent) LOG4CPLUS_FATAL_STR(logger, logEvent)
#endif //LOG_FATAL_STR

#endif //SRC_LIB_COMMON_LOGGER_H
