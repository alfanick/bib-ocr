#include "extractor.h"

namespace bib_ocr {

Extractor::Extractor(const std::string& filename) : filename_(filename) {
  image_ = ImageHandler::ReadOriented(filename);
  // TODO(kareth) read image_handler header file
  ImageHandler::set_filename(filename);
}

Extractor::~Extractor() {
}

int Extractor::Extract() {
  BlockSeparator separator(image_);
  if (separator.Separate() == -1)
    return -1;

  ExtractNumbers(separator.GetBlocks());
  return 0;
}

void Extractor::ExtractNumbers(const std::vector<std::pair<cv::Mat, cv::Mat> >& blocks) {
  for (auto block : blocks) {
    NumberReader reader(block);
    if (reader.Read() == 0)
      AddResult(reader.GetNumbers());
  }
}

void Extractor::AddResult(const std::vector<Result>& results) {
  using std::swap;
  for (auto result : results) {
    int found = 0;
    for (auto existing : numbers_) {
      if (existing.number() == result.number()) {
        if (existing.probability() < result.probability())
          swap(existing, result);
        found = 1;
        break;
      }
    }
    if (!found)
      numbers_.push_back(result);
  }
  return;
}

}  // namespace bib_ocr
