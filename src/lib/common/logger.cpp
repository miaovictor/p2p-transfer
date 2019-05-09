//
// Created by victor on 19-3-5.
//

#include "logger.h"
#include "log4cplus/logger.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/asyncappender.h"
#include "log4cplus/fileappender.h"

namespace vzc {

log4cplus::Logger logger;

bool InitializeLogger(bool tofile /*= false*/,
                      const std::string &file_path /*= ""*/) {
  log4cplus::initialize();
  log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);

  std::string console_pattern = "%D{%m/%d %H:%M:%S.%q} %-5p - %m [%l]%n";
  log4cplus::SharedAppenderPtr control_appender(new log4cplus::ConsoleAppender());
  control_appender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(console_pattern)));

  log4cplus::AsyncAppenderPtr async_appender(new log4cplus::AsyncAppender(
          log4cplus::SharedAppenderPtr(control_appender.get()), 2000));

  if (tofile && !file_path.empty()) {
    log4cplus::SharedFileAppenderPtr file_appender(
        new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT(file_path),
                                           10 * 1024 * 1024, 10, true, true));
    std::string file_pattern = "%D{%m/%d %H:%M:%S.%q} %-5p - %m [%l]%n";
    file_appender->setLayout(std::unique_ptr<log4cplus::Layout>(
        new log4cplus::PatternLayout(file_pattern)));
    async_appender->addAppender(
        log4cplus::SharedAppenderPtr(file_appender.get()));

  }

  logger = log4cplus::Logger::getRoot();

  logger.addAppender(log4cplus::SharedAppenderPtr(async_appender.get()));

  return true;
}

void UnInitializeLogger() {
  logger.shutdown();
}

}



