#ifndef COLOR_REPLACER_H_
#define COLOR_REPLACER_H_

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bib_ocr {

class ColorReplacer {
  public:
    // Provide image with HSV color space
    explicit ColorReplacer(const cv::Mat& image);
    ~ColorReplacer();

    cv::Mat& GetImage() { return image_; };
    void Replace(const cv::Scalar start, const cv::Scalar end, const cv::Scalar to);
    void White(const cv::Scalar start, const cv::Scalar end);
    void Black(const cv::Scalar start, const cv::Scalar end);
  private:
    cv::Mat image_;

};

}

#endif
