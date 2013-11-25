#ifndef TESSERACT_PARSER_H_
#define TESSERACT_PARSER_H_

#include <vector>
#include <string>
#include <iostream>

#include "result.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace bib_ocr {

class TesseractParser {
 public:
  explicit TesseractParser(cv::Mat* image);
  ~TesseractParser();

  int Parse();
  Result GetResult() const;

 private:
  int ProcessWord(const std::string& word, int probability);
  int ParseWords(char* words, int* probabilities);

  cv::Mat* image_;
  std::vector<Result> results_;
};

}  // namespace bib_ocr

#endif  // TESSERACT_PARSER_H_
