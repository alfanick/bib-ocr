#ifndef TRAVERSER_H_
#define TRAVERSER_H_

#include <stack>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class Traverser {
 public:
  // Does not take ownership
  // TODO(kareth) broken style
  Traverser(const cv::Mat& original, const cv::Mat& edges, std::vector<std::vector<bool> >* visited);
  ~Traverser();

  void Run(int x, int y);

  cv::Range RowRange() const;
  cv::Range ColRange() const;

  const std::vector<std::pair<int, int> >& points() const { return points_; }


 private:
  const cv::Mat* original_;
  const cv::Mat* edges_;

  std::vector<std::vector<bool> >* visited_;

  std::vector<std::pair<int, int> > points_;

  static int move_x_[];
  static int move_y_[];
};

}  // namespace bib_ocr

#endif  // TRAVERSER_H_
