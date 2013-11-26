#ifndef PROMISING_AREAS_H_
#define PROMISING_AREAS_H_

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class PromisingAreas {
 public:
  PromisingAreas(const cv::Mat& original, const cv::Mat& bw);
  ~PromisingAreas();

  std::vector<cv::Mat> GetAreas();

 private:
  cv::Mat original_;
  cv::Mat bw_;
};

}  // namespace bib_ocr

#endif  // PROMISING_AREAS_H_
