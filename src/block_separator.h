#ifndef BLOCK_SEPARATOR_H_
#define BLOCK_SEPARATOR_H_

#include <vector>
#include <string>

#include "image_handler.h"
#include "traverser.h"
#include "exif.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

// Separates blocks that could contain numbers
class BlockSeparator {
 public:
   // Does not take ownership
   explicit BlockSeparator(const cv::Mat& image);
   ~BlockSeparator();

   int Separate();
   std::vector<std::pair<cv::Mat, cv::Mat> > GetBlocks() const { return blocks_; }

 private:
  void FilterGroups();
  void FindEdges();
  void MarkPromisingAreas();
  bool IsWhite(const cv::Mat& image, int x, int y) const;
  void SetColor(cv::Mat* image, int x, int y, int r, int g, int b);
  void FindPromisingAreas();
  void AddBlock(const cv::Range& rows, const cv::Range& cols, const std::vector<std::pair<int, int> >& points);
  void SaveBlocks() const;

  std::vector<std::pair<cv::Mat, cv::Mat> > ExtractSubBlocks(const cv::Mat& input);

  const double kContrast = 1.5;
  const int kBrightness = 0;
  const int kMinBlockSize = 5000;
  cv::Mat original_;
  cv::Mat filtered_;
  cv::Mat edges_;
  cv::Mat canny_;
  cv::Mat markup_;
  cv::Mat groups_;

  std::vector<std::pair<cv::Mat, cv::Mat> > blocks_;
  std::vector<std::vector<bool> > visited_;

};

}  // namespace bib_ocr

#endif  // BLOCK_SEPARATOR_H_
