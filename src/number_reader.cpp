#include "number_reader.h"

namespace bib_ocr {

NumberReader::NumberReader(std::pair<cv::Mat, cv::Mat>& block)
  : original_(&block.first), bw_(&block.second) {
}

NumberReader::~NumberReader() {
}

int NumberReader::Read() {
  PromisingAreas areas(*original_, *bw_);
  areas.Compute();
  TesseractParser parser;

  for (auto image : areas.areas())
    parser.Parse(image);

  if (parser.resultFound()) {
    result_ = parser.GetResult();
    return 0;
  } else {
    return -1;
  }
}

}  // namespace bib_ocr
