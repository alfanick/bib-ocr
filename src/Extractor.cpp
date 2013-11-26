#include "extractor.h"

namespace bib_ocr {

Extractor::Extractor(const std::string& filename) : filename_(filename) {
  image_ = ImageHandler::ReadOriented(filename);
  ImageHandler::set_filename(filename);
}

Extractor::Extractor(const std::string& filename, const cv::Scalar black_start, const cv::Scalar black_end, const cv::Scalar white_start, const cv::Scalar white_end) : filename_(filename) {
  image_ = ImageHandler::ReadOriented(filename);
  cv::cvtColor(image_, image_, CV_BGR2HSV);

  std::unique_ptr<ColorReplacer> cr = std::unique_ptr<ColorReplacer>(new ColorReplacer(image_));
  cr->Black(black_start, black_end);
  cr->White(white_start, white_end);
  image_ = cr->GetImage();
  cv::cvtColor(image_, image_, CV_HSV2BGR);
  ImageHandler::set_filename(filename);
}

Extractor::~Extractor() {
}

int Extractor::Extract() {
  BlockSeparator separator(image_);
  if (separator.Separate() == -1)
    return -1;

  ExtractNumbers(separator.GetBlocks());
  return 0;
}

void Extractor::ExtractNumbers(const std::vector<std::pair<cv::Mat, cv::Mat> >& blocks) {
  for (auto block : blocks) {
    NumberReader reader(&block.first);
    if (reader.Read() == 0)
      AddResult(reader.GetNumber());
  }
}

void Extractor::AddResult(const Result& result) {
  numbers_.push_back(result);
  return;
}

}  // namespace bib_ocr
