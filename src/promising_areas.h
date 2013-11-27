#ifndef PROMISING_AREAS_H_
#define PROMISING_AREAS_H_

#include <vector>

#include "traverser.h"
#include "image_handler.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class PromisingAreas {
 public:
  PromisingAreas(const cv::Mat& original, const cv::Mat& bw);
  ~PromisingAreas();

  void Compute();
  const std::vector<cv::Mat>& areas() const { return areas_; }

 private:
  bool IsWhite(const cv::Mat& image, int x, int y) const;
  void SetColor(cv::Mat* image, int x, int y, int r, int g, int b);
  void GenerateAreas();

  cv::Mat original_;
  cv::Mat bw_;
  std::vector<cv::Mat> areas_;
  std::vector<std::vector<bool> > visited_;

  std::vector<std::vector<std::pair<int, int> > > constituents_;
};

}  // namespace bib_ocr

#endif  // PROMISING_AREAS_H_
