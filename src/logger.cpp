#include "logger.h"

namespace bib_ocr {

Log* Log::log_ = NULL;

Log::Log() {
  handle_ = stderr;
}

Log::Log(std::string name) {
  handle_ = fopen(name.c_str(), "w");
}

Log::~Log() {
  fclose(handle_);
}

void Log::file(std::string name) {
  log_ = new Log(name);
}

void Log::write(std::string prefix, char* message) {
  fprintf(handle_, "[%s] %s\n", prefix.c_str(), message);
}

LOG_BODY_(info);
LOG_BODY_(extractor);
LOG_BODY_(block_separator);

}
