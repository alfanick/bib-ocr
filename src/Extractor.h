#ifndef BO_EXTRACTOR_H
#define BO_EXTRACTOR_H

#include <string>
#include <opencv2/imgproc/imgproc.hpp>

namespace BibOcr {
  class Extractor {
    private:
      cv::Mat image;

    public:
      Extractor();
      ~Extractor();
  };
}

#endif
