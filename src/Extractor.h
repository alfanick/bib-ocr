#ifndef BO_EXTRACTOR_H
#define BO_EXTRACTOR_H

#include <string>
#include <vector>
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

      std::vector<cv::Mat> extract();

    private:
      void filter();

      void save(std::string category, cv::Mat data);
  };
}

#endif
