#ifndef LOGGER_H_
#define LOGGER_H_

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <chrono>

#define LOG_HEADER_(context) static void context(std::string format, ...)
#define LOG_BODY_(context) void Log::context(std::string format, ...) { \
  if (log_ == NULL) \
    log_ = new Log(); \
  va_list args; \
  va_start(args, format); \
  char buffer[1024]; \
  vsnprintf(buffer, 1024, format.c_str(), args); \
  log_->write(#context, buffer);\
}

namespace bib_ocr {

class Log {
 public:
  Log();
  Log(std::string name);
  ~Log();
  static void file(std::string name);
  void write(std::string prefix, char* message);

  LOG_HEADER_(info);
  LOG_HEADER_(extractor);
  LOG_HEADER_(block_separator);
  LOG_HEADER_(tesseract_parser);
 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  static Log* log_;
  FILE* handle_;
};

}

#endif
