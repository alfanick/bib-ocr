#ifndef NUMBER_READER_H_
#define NUMBER_READER_H_

#include <vector>
#include "result.h"
#include "tesseract_parser.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class NumberReader {
 public:
  // Does not take ownership
  explicit NumberReader(std::pair<cv::Mat, cv::Mat>& blocks);
  ~NumberReader();

  int Read();
  std::vector<Result> GetNumbers() const { return result_; }

 private:
  cv::Mat* original_;
  cv::Mat* bw_;
  std::vector<Result> result_;
};

}  // namespace bib_ocr

#endif  // NUMBER_READER_H_
