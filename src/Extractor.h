#ifndef BO_EXTRACTOR_H
#define BO_EXTRACTOR_H

#include <string>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace BibOcr {
  class Extractor {
    private:
      std::string filename;
      cv::Mat image;

    public:
      Extractor(const std::string f);
      ~Extractor();
  };
}

#endif
