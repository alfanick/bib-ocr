#include "image_handler.h"

namespace bib_ocr {

std::string ImageHandler::filename_;

cv::Mat ImageHandler::ReadOriented(const std::string& filename) {
  FILE *fp = fopen(filename.c_str(), "rb");
  fseek(fp, 0, SEEK_END);
  unsigned long fsize = ftell(fp);
  rewind(fp);
  unsigned char *buf = new unsigned char[fsize];
  fread(buf, 1, fsize, fp);

  fclose(fp);

  EXIFInfo result;
  result.parseFrom(buf, fsize);
  delete[] buf;

  cv::Mat image = cv::imread(filename);

  if (result.Orientation > 1) {
    switch (result.Orientation) {
      case 6:
        rotate_image_90n(image, image, 90);
        break;
      case 8:
        rotate_image_90n(image, image, 270);
        break;
      case 3:
        rotate_image_90n(image, image, 180);
        break;
    }
  }


  return image;
}

void ImageHandler::Save(const std::string& category, const cv::Mat& data) {
  std::string filename(filename_);

  filename.replace(filename.find("input/"), 6, "output/");
  filename.replace(filename.find(".jpg"), 4, "." + category + ".jpg");

  cv::imwrite(filename, data);
}

void ImageHandler::rotate_image_90n(cv::Mat &src, cv::Mat &dst, int angle) {
  if (src.data != dst.data)
    src.copyTo(dst);

  angle = ((angle / 90) % 4) * 90;

  bool const flip_horizontal_or_vertical = angle > 0 ? 1 : 0;
  int const number = std::abs(angle / 90);
  for(int i = 0; i != number; ++i){
    cv::transpose(dst, dst);
    cv::flip(dst, dst, flip_horizontal_or_vertical);
  }
}

}  // namespace bib_ocr
