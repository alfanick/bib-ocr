#include "image_handler.h"

namespace bib_ocr {

std::string ImageHandler::filename_;

void ImageHandler::Save(const std::string& category, const cv::Mat& data) {
  std::string filename(filename_);

  filename.replace(filename.find("input/"), 6, "output/");
  filename.replace(filename.find(".jpg"), 4, "." + category + ".jpg");

  cv::imwrite(filename, data);
}

}  // namespace bib_ocr
