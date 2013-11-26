#include "color_replacer.h"

namespace bib_ocr {

ColorReplacer::ColorReplacer(const cv::Mat& image) {
  image_ = image.clone();
}

ColorReplacer::~ColorReplacer() {

}

void ColorReplacer::White(const cv::Scalar start, const cv::Scalar end) {
  Replace(start, end, cv::Scalar(0, 0, 255));
}

void ColorReplacer::Black(const cv::Scalar start, const cv::Scalar end) {
  Replace(start, end, cv::Scalar(0, 0, 0));
}

void ColorReplacer::Replace(const cv::Scalar start, const cv::Scalar end, const cv::Scalar to) {
  cv::Mat mask;
  cv::inRange(image_, start, end, mask);
  image_.setTo(to, mask);
}

}
