#include "block_separator.h"

namespace bib_ocr {

BlockSeparator::BlockSeparator(const cv::Mat& original)
  : blocks_(), visited_() {
  original_ = original.clone();
  edges_ = original.clone();
  markup_ = original.clone();
  groups_ = original.clone();
  std::vector<bool> row(original.cols, false);
  visited_.resize(original.rows, row);
}

BlockSeparator::~BlockSeparator() {
}

int BlockSeparator::Separate() {
  ImageHandler::Save("raw", original_);
  printf("Searching for blocks...\n");
  FilterGroups();
  FindEdges();
  MarkPromisingAreas();
  FindPromisingAreas();
  printf("Found %lu blocks!\n", blocks_.size());
  SaveBlocks();
  return 0;
}

void BlockSeparator::FilterGroups() {
  for (int y = 0; y < groups_.rows; y++) {
    for (int x = 0; x < groups_.cols; x++) {
      for (int c = 0; c < 3; c++) {
        groups_.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>(groups_.at<cv::Vec3b>(y,x)[c] * kContrast + kBrightness);
      }
    }
  }

  filtered_ = groups_.clone();

  ImageHandler::Save("bc", groups_);
}

void BlockSeparator::MarkPromisingAreas() {
  cv::Mat background_mask, output_mask, contours_mask;
  cv::Mat background, output, sharp;

  std::vector<std::vector<cv::Point> > contours, number_contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::Mat edges = edges_.clone();

  printf("Finding objects\n");
  // find contours
  cv::findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);


  printf("Removing small objects\n");
  for (auto h = hierarchy.begin(); h != hierarchy.end(); ++h) {
    // has childs - not number
    if ((*h)[2] != -1) {
      contours.erase(contours.begin() + (h-hierarchy.begin()));
    }
  }

  // filter small figures
  for (auto contour : contours) {
    double area = cv::contourArea(cv::Mat(contour));

    if (area < 3000)
      continue;
    number_contours.push_back(contour);
  }

  contours_mask = cv::Mat::zeros(groups_.rows, groups_.cols, CV_8UC1);
  cv::drawContours(contours_mask, number_contours, -1, cv::Scalar(255), CV_FILLED);


  ImageHandler::Save("sharpmask", contours_mask);
  groups_.copyTo(sharp, contours_mask);

  ImageHandler::Save("sharp", sharp);

  cv::cvtColor(sharp, sharp, CV_BGR2HLS);

  cv::inRange(sharp, cv::Scalar(0, 120, 0), cv::Scalar(255, 255, 70), background_mask);
  groups_.copyTo(background, background_mask);

  cv::inRange(background, cv::Scalar(0, 0, 0), cv::Scalar(255, 70, 100), output_mask);

  groups_.setTo(cv::Scalar(255,255,255));
  background.copyTo(groups_, output_mask);

  ImageHandler::Save("groups", groups_);
}

bool BlockSeparator::IsWhite(const cv::Mat& image, int x, int y) const {
  int a[3];

  for (int i = 0; i < 3; i++)
    a[i] = image.data[image.channels() * (image.cols * x + y) + i];

  for(int i = 0; i < 3; i++)
    if (abs(a[i] - a[(i + 1) % 3]) > 55)
      return false;

  if (a[0] + a[1] + a[2] >= 3 * 200)
    return true;
  else
    return false;
}

void BlockSeparator::SetColor(cv::Mat* image, int x, int y, int r, int g, int b) {
  int a[] = {r, g, b};
  for (int i = 0; i < 3; i++)
    image->data[image->channels() * (image->cols * x + y) + i] = a[i];
}

void BlockSeparator::FindEdges() {
  printf("Finding edges...\n");
  cv::cvtColor(edges_, edges_, CV_BGR2GRAY);
  cv::blur(edges_, edges_, cv::Size(3,3));

  cv::Canny(edges_, edges_, 60, 200);

  cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,7), cv::Point(3,3));
  for (int i =0; i < 5; i++)
    cv::dilate(edges_, edges_, dilate_kernel);



  canny_ = edges_.clone();

  ImageHandler::Save("canny", edges_);
}

void BlockSeparator::FindPromisingAreas() {
  markup_ = original_.clone();
  for (int i = 0; i < groups_.rows; i++)
    for (int j = 0; j < groups_.cols; j++)
      if (!IsWhite(groups_, i, j))
        visited_[i][j] = true;


  printf("Exploring objects\n");
  for (int i = 0; i < groups_.rows; i++)
    for (int j = 0; j < groups_.cols; j++)
      if (!visited_[i][j]) {
        Traverser traverser(groups_, edges_, &visited_);
        traverser.Run(i, j);

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
      }

  ImageHandler::Save("markup", markup_);
}

void BlockSeparator::AddBlock(const cv::Range& rows, const cv::Range& cols, const std::vector<std::pair<int, int> >& points) {
  if (points.size() < kMinBlockSize)
    return;

  cv::Mat block = cv::Mat(original_,rows,cols).clone();
  cv::cvtColor(block, block, CV_BGR2HSV);

  cv::Mat block_mask = cv::Mat(block.rows, block.cols, CV_8UC1);

  cv::inRange(block, cv::Scalar(0, 0, 0), cv::Scalar(255, 100, 90), block_mask);
  cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,7), cv::Point(3,3));
  for (int i =0; i < 5; i++)
    cv::dilate(block_mask, block_mask, dilate_kernel);

  std::vector<std::vector<cv::Point> > contours, number_contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(block_mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

  std::vector<double> areas;
  double area_total = block.rows * block.cols;

  for (auto contour : contours)
    areas.push_back(cv::contourArea(contour));

  cv::cvtColor(block, block, CV_HSV2BGR);

  cv::Mat blurred;
  ImageHandler::Save("befores", block);
  cv::GaussianBlur(block, blurred, cv::Size(0, 0), 5);
  cv::addWeighted(block, 1.5, blurred, -0.5, 0, block);
  ImageHandler::Save("afters", block);

  int i = 0;
  for (auto contour : contours) {
    if (areas[i++]/area_total < 0.05) {
      printf("  too small\n");
      continue;
    }
    printf("  found\n");
    cv::Mat inner_block = block.clone();
    cv::Mat inner_mask = cv::Mat::zeros(block.rows, block.cols, CV_8UC1);
    cv::drawContours(inner_mask, std::vector<std::vector<cv::Point> > (1, contour), -1, cv::Scalar(255), CV_FILLED);

    inner_block.setTo(cv::Scalar(255,255,255));
    block.copyTo(inner_block, inner_mask);

    cv::cvtColor(inner_block, inner_block, CV_BGR2GRAY);
    cv::threshold(inner_block, inner_block, 100, 255, 0);
    cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3), cv::Point(1,1));
    for (int i =0; i < 1; i++)
      cv::dilate(inner_block, inner_block, dilate_kernel);
    cv::erode(inner_block, inner_block, dilate_kernel);

    blocks_.push_back(std::make_pair(inner_block, cv::Mat(original_, rows, cols).clone()));
  }


  // Draw rectangle
  int color[] = { 255, (rand() % 2) * 255, (rand() % 2) * 255 };

  for(int i = rows.start; i < rows.end; i++) {
    SetColor(&markup_, i, cols.start, color[0], color[1], color[2]);
    SetColor(&markup_, i, cols.start + 1, color[0], color[1], color[2]);
    SetColor(&markup_, i, cols.end, color[0], color[1], color[2]);
    SetColor(&markup_, i, cols.end - 1, color[0], color[1], color[2]);
  }

  for(int i = cols.start; i < cols.end; i++) {
    SetColor(&markup_, rows.start, i, color[0], color[1], color[2]);
    SetColor(&markup_, rows.start + 1, i, color[0], color[1], color[2]);
    SetColor(&markup_, rows.end, i, color[0], color[1], color[2]);
    SetColor(&markup_, rows.end - 1, i, color[0], color[1], color[2]);
  }
}

void BlockSeparator::SaveBlocks() const {
  for (int i = 0; i < blocks_.size(); i++) {
    ImageHandler::Save("block_" + std::to_string(i), blocks_[i].first);
    ImageHandler::Save("block_original_" + std::to_string(i), blocks_[i].second);
  }
}

}  // namespace bib_ocr
