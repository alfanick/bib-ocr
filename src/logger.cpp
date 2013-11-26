#include "logger.h"

namespace bib_ocr {

Log* Log::log_ = NULL;

Log::Log() {
  handle_ = stderr;

  start_time_ = std::chrono::high_resolution_clock::now();
}

Log::Log(std::string name) {
  Log();
  handle_ = fopen(name.c_str(), "w");
}

Log::~Log() {
  fclose(handle_);
}

void Log::file(std::string name) {
  log_ = new Log(name);
}

void Log::write(std::string prefix, char* message) {
  auto point = std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(point - start_time_);
  fprintf(handle_, "[%s@%.3f] %s\n", prefix.c_str(), double(ms.count())/1000.0, message);
}

LOG_BODY_(info);
LOG_BODY_(extractor);
LOG_BODY_(block_separator);

}
