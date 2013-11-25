#include "number_reader.h"

namespace bib_ocr {

NumberReader::NumberReader(cv::Mat* image)
  : image_(image), result_() {
}

NumberReader::~NumberReader() {
}

int NumberReader::Read() {
  TesseractParser parser(image_);
  if(parser.Parse() != -1) {
    result_ = parser.GetResult();
    return 0;
  } else {
    return -1;
  }
}

}  // namespace bib_ocr
