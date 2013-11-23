#ifndef BLOCK_SEPARATOR_H_
#define BLOCK_SEPARATOR_H_

#include <vector>

#include "image_handler.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

// Separates blocks that could contain numbers
class BlockSeparator {
 public:
   // Does not take ownership
   explicit BlockSeparator(cv::Mat* image);
   ~BlockSeparator();

   int Separate();
   std::vector<cv::Mat> GetBlocks() const { return blocks_; }

 private:
  void Filter();

   cv::Mat* image_;
   std::vector<cv::Mat> blocks_;

};

}  // namespace bib_ocr

#endif  // BLOCK_SEPARATOR_H_
