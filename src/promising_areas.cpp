#include "promising_areas.h"

namespace bib_ocr {

PromisingAreas::PromisingAreas(const cv::Mat& original, const cv::Mat& bw) {
  original_ = original.clone();
  bw_ = bw.clone();
}

PromisingAreas::~PromisingAreas() {
}

std::vector<cv::Mat> PromisingAreas::GetAreas() {
}

}  // bib_ocr
