#ifndef IMAGE_HANDLER_H_
#define IMAGE_HANDLER_H_

#include <string>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "exif.h"

namespace bib_ocr {

class ImageHandler {
 public:
  static void Save(const std::string& category, const cv::Mat& image);
  static cv::Mat ReadOriented(const std::string& filename);

  static void set_filename(const std::string& filename) { filename_ = filename; }

 private:
  // TODO(kareth) I dont like this solution, but as far as its used
  // only for debug, it can stay here for a while
  static std::string filename_;

  // http://stackoverflow.com/questions/16265673/rotate-image-by-90-180-or-270-degrees/16278334#16278334
  static void rotate_image_90n(cv::Mat &src, cv::Mat &dst, int angle);
};

}  // namespace bib_ocr

#endif  // IMAGE_HANDLER_H_
