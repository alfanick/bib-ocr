#include "number_reader.h"

namespace bib_ocr {

NumberReader::NumberReader(cv::Mat* image)
  : image_(image), result_() {
}

NumberReader::~NumberReader() {
}

int NumberReader::Read() {
  // TODO(kareth) Read number from image
  return 0;
}

}  // namespace bib_ocr
