#include "promising_areas.h"

namespace bib_ocr {

PromisingAreas::PromisingAreas(const cv::Mat& bw, const cv::Mat& original)
  : areas_(), visited_(), constituents_() {
  original_ = original.clone();
  bw_ = bw.clone();
  std::vector<bool> row(original.cols, false);
  visited_.resize(original.rows, row);
}

PromisingAreas::~PromisingAreas() {
}

bool PromisingAreas::IsWhite(const cv::Mat& image, int x, int y) const {
  for (int i = 0; i < 3; i++) {
    //printf("%d\n", image.data[image.channels() * (image.cols * x + y) + i]);
    if (image.data[image.channels() * (image.cols * x + y) + i] < 120)
      return false;
  }
  return true;
}

void PromisingAreas::SetColor(cv::Mat* image, int x, int y, int r, int g, int b) {
  int a[] = {r, g, b};
  for (int i = 0; i < 3; i++)
    image->data[image->channels() * (image->cols * x + y) + i] = a[i];
}

int indexpro = 0;

void PromisingAreas::Compute() {
  srand(time(0));
  areas_.push_back(bw_);

  cv::Mat markup = bw_.clone();
  cv::cvtColor(markup, markup, CV_GRAY2RGB);

  for (int i = 0; i < bw_.rows; i++)
    for (int j = 0; j < bw_.cols; j++)
      if (IsWhite(bw_, i, j)){
        visited_[i][j] = true;
      }

  for (int i = 0; i < bw_.rows; i++)
    for (int j = 0; j < bw_.cols; j++)
      if (!visited_[i][j]) {
        //printf("dupa\n");
        Traverser traverser(bw_, bw_, &visited_);
        traverser.Run(i, j);
/*
        double height = traverser.ColRange().size();
        double width = traverser.RowRange().size();

        if (height <= 0 || width <= 0)
          continue;

        if (height > width)
          std::swap(height, width);

        if (width/height > 1.7)
          continue;

//        printf("%f\n", height/width);

        AddBlock(traverser.RowRange(), traverser.ColRange(), traverser.points());
*/
        if (traverser.points().size() > 100) {
          constituents_.push_back(traverser.points());
          int color[] = {(rand() % 3 + 1) * 80, (rand() % 4) * 80, (rand() % 4) * 80 };
          for (auto point : traverser.points())
            SetColor(&markup, point.first, point.second, color[0], color[1], color[2]);
        }
      }
  ImageHandler::Save("promising" + std::to_string(indexpro++), markup);
  GenerateAreas();
}

void PromisingAreas::GenerateAreas() {
  //if (constituents_.size() > 3) return;
  //
  std::sort(constituents_.begin(), constituents_.end(), [](std::vector<std::pair<int, int> > a, std::vector<std::pair<int, int> > b) { return a.size() > b.size(); });
  while(constituents_.size() > 6) constituents_.pop_back();

  cv::Mat white = bw_.clone();
  cv::cvtColor(white, white, CV_GRAY2RGB);

  for (int i = 0; i < white.rows; i++)
    for (int j = 0; j < white.cols; j++)
      SetColor(&white, i, j, 255, 255, 255);


  for (int i = 0; i < (1<<constituents_.size()); i++) {
    cv::Mat mask = white.clone();

    for (int j = 0; j < constituents_.size(); j++) {
      if (i & (1<<j) ) {
        for (auto point : constituents_[j]) {
          SetColor(&mask, point.first, point.second, 0, 0, 0);
        }
      }
    }

    cv::Mat area = original_.clone();
    for (int x = 0; x < mask.rows; x++) {
      for (int y = 0; y < mask.cols; y++) {
        if (IsWhite(mask, x, y))
          SetColor(&area, x, y, 255, 255, 255);
      }
    }
    areas_.push_back(area);
  }
}

}  // bib_ocr
