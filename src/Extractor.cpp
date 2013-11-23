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


    save("canny", image_);

    return objects;
  }

  void Extractor::filter() {
    cv::Canny(image_, image_, 20, 100, 3);

  }

  void Extractor::save(std::string category, cv::Mat data) {
    std::string filename = filename_.replace(filename_.find("input/"), 6, "output/");
    filename = filename.replace(filename.find(".jpg"), 4, "." + category + ".jpg");

    cv::imwrite(filename, data);
  }
}
