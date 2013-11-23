#ifndef BO_EXTRACTOR_H
#define BO_EXTRACTOR_H

#include <string>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace BibOcr {
  class Extractor {
    private:
      std::string filename_;
      cv::Mat image_;

    public:
      Extractor(const std::string filename);
      ~Extractor();
  };
}

#endif
