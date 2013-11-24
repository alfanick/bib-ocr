#include "traverser.h"

namespace bib_ocr {

int Traverser::move_x_[] = {0, 1, 0, -1};
int Traverser::move_y_[] = {1, 0, -1, 0};

Traverser::Traverser(const cv::Mat& original, const cv::Mat& edges, std::vector<std::vector<bool> >* visited)
  : original_(&original), edges_(&edges), visited_(visited) {
}

Traverser::~Traverser() {
}

void Traverser::Run(int x, int y) {
  if (x < 0 || y < 0 || x >= original_->rows || y >= original_->cols)
    return;
  if ((*visited_)[x][y] == true) return;

  (*visited_)[x][y] = true;
  points_.push_back(std::pair<int, int>(x, y));

  for (int i = 0; i < 4; i++)
    Run(x + move_x_[i], y + move_y_[i]);
}


// TODO(kareth) inefficent
cv::Range Traverser::RowRange() const {
  int start = 999999, end = 0;
  for (auto point : points_) {
    start = std::min(start, point.first);
    end = std::max(end, point.first);
  }
  return cv::Range(start, end);
}

cv::Range Traverser::ColRange() const {
  int start = 999999, end = 0;
  for (auto point : points_) {
    start = std::min(start, point.second);
    end = std::max(end, point.second);
  }
  return cv::Range(start, end);
}

}  // namespace bib_ocr
