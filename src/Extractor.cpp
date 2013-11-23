#include "Extractor.h"

namespace BibOcr {
  Extractor::Extractor(const std::string filename) : filename_(filename) {
    image_ = cv::imread(filename);
  }

  Extractor::~Extractor() {
  }
}
