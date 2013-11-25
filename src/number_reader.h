#ifndef NUMBER_READER_H_
#define NUMBER_READER_H_

#include "result.h"
#include "tesseract_parser.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class NumberReader {
 public:
  // Does not take ownership
  explicit NumberReader(cv::Mat* image);
  ~NumberReader();

  int Read();
  Result GetNumber() const { return result_; }

 private:
  cv::Mat* image_;
  Result result_;
};

}  // namespace bib_ocr

#endif  // NUMBER_READER_H_
