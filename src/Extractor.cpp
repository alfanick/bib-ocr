#include "Extractor.h"

namespace BibOcr {
  Extractor::Extractor(const std::string f) : filename(f) {
    image = cv::imread(filename);
  }

  Extractor::~Extractor() {
  }
}
