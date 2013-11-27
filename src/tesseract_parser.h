#ifndef TESSERACT_PARSER_H_
#define TESSERACT_PARSER_H_

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "result.h"
#include "promising_areas.h"
#include "logger.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace bib_ocr {

class TesseractParser {
 public:
  TesseractParser();
  ~TesseractParser();

  int Parse(const cv::Mat& image);
  std::vector<Result> GetResult();
  int resultFound() const { return results_.size() > 0; }

 private:
  int ProcessWord(const std::string& word, int probability);
  int ParseWords(char* words, int* probabilities);

  void AddResult(int number, int probability);
  void FilterResults();

  std::vector<Result> results_;
  tesseract::TessBaseAPI tess_;
};

}  // namespace bib_ocr

#endif  // TESSERACT_PARSER_H_
