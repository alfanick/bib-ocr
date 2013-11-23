#include "block_separator.h"

namespace bib_ocr {

BlockSeparator::BlockSeparator(cv::Mat* image)
  : image_(image), blocks_() {
}

BlockSeparator::~BlockSeparator() {
}

int BlockSeparator::Separate() {
  Filter();
  return 0;
}

void BlockSeparator::Filter() {
  cv::Canny(*image_, *image_, 120, 200, 3);
  ImageHandler::Save("canny", *image_);
}

}  // namespace bib_ocr
