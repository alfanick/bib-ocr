#include "Extractor.h"

namespace BibOcr {
  Extractor::Extractor(const std::string filename) : filename_(filename) {
    image_ = cv::imread(filename);

    filter();
  }

  Extractor::~Extractor() {
  }

  std::vector<cv::Mat> Extractor::extract() {
    std::vector<cv::Mat> objects;


    return objects;
  }

  void Extractor::filter() {
    cv::Canny(image_, image_, 120, 200, 3);
    save("canny", image_);

  }

  void Extractor::save(const std::string category, const cv::Mat data) {
    std::string filename(filename_);

    filename.replace(filename.find("input/"), 6, "output/");
    filename.replace(filename.find(".jpg"), 4, "." + category + ".jpg");

    cv::imwrite(filename, data);
  }
}
