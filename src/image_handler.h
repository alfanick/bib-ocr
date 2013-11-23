#ifndef IMAGE_HANDLER_H_
#define IMAGE_HANDLER_H_

#include <string>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class ImageHandler {
 public:
  static void Save(const std::string& category, const cv::Mat& image);

  static void set_filename(const std::string& filename) { filename_ = filename; }

 private:
  // TODO(kareth) I dont like this solution, but as far as its used
  // only for debug, it can stay here for a while
  static std::string filename_;
};

}  // namespace bib_ocr

#endif  // IMAGE_HANDLER_H_
