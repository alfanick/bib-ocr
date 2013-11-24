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
  std::stack< std::pair<int, int> > s;
  s.push(std::make_pair(x, y));

  while (!s.empty()) {
    std::pair<int, int> current = s.top();
    s.pop();

    if (current.first < 0 || current.second < 0 || current.first >= original_->rows || current.second >= original_->cols)
      continue;
    if ((*visited_)[current.first][current.second])
      continue;

    (*visited_)[current.first][current.second] = true;

    points_.push_back(current);

    for (int i = 0; i < 4; i++)
      s.push(std::make_pair(current.first + move_x_[i], current.second + move_y_[i]));
  }
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
