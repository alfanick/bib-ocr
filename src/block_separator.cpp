#include "block_separator.h"

namespace bib_ocr {

BlockSeparator::BlockSeparator(const cv::Mat& original)
  : blocks_(), visited_() {
  original_ = original.clone();
  edges_ = original.clone();
  markup_ = original.clone();
  groups_ = original.clone();
  sharp_ = original.clone();
  filtered_ = original.clone();
  canny_ = original.clone();
  std::vector<bool> row(original.cols, false);
  visited_.resize(original.rows, row);
}

BlockSeparator::~BlockSeparator() {
}

int BlockSeparator::Separate() {
  ImageHandler::Save("raw", original_);
  Log::block_separator("Searching for blocks...");
  FilterGroups();

  Log::block_separator("Edge detection");
  FindEdges();

  Log::block_separator("Marking promising areas");
  MarkPromisingAreas();

  Log::block_separator("Finding edges");
  FindPromisingAreas();

  Log::block_separator("Found %lu blocks!", blocks_.size());
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

  // find contours
  cv::findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
  Log::block_separator("Detected %lu contours", contours.size());


  Log::block_separator("Removing small objects");
  for (auto h = hierarchy.begin(); h != hierarchy.end(); ++h) {
    // has childs - not number
    if ((*h)[2] != -1) {
      contours.erase(contours.begin() + (h-hierarchy.begin()));
    }
  }

  // filter small figures
  for (auto contour : contours) {
    double area = cv::contourArea(cv::Mat(contour));

    if (area < 3000) {
      Log::block_separator("  removed area = %f", area);
      continue;
    }
    number_contours.push_back(contour);
  }

  contours_mask = cv::Mat::zeros(groups_.rows, groups_.cols, CV_8UC1);
  cv::drawContours(contours_mask, number_contours, -1, cv::Scalar(255), CV_FILLED);


  ImageHandler::Save("sharpmask", contours_mask);
  groups_.copyTo(sharp, contours_mask);

  sharp_ = groups_.clone();
  sharp_.setTo(cv::Scalar(255,255,255));
  groups_.copyTo(sharp_, contours_mask);

  ImageHandler::Save("sharp", sharp);

  cv::cvtColor(sharp, sharp, CV_BGR2HLS);

  cv::inRange(sharp, cv::Scalar(0, 120, 0), cv::Scalar(255, 255, 70), background_mask);
  groups_.copyTo(background, background_mask);

  cv::inRange(background, cv::Scalar(0, 0, 0), cv::Scalar(255, 70, 100), output_mask);
  groups_.setTo(cv::Scalar(255,255,255));
  background.copyTo(groups_, output_mask);

  Log::block_separator("Masked groups");

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
  cv::cvtColor(edges_, edges_, CV_BGR2GRAY);
  cv::blur(edges_, edges_, cv::Size(3,3));

  cv::Canny(edges_, edges_, 60, 200);
  ImageHandler::Save("canny", edges_);

  cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,7), cv::Point(3,3));
  for (int i =0; i < 5; i++)
    cv::dilate(edges_.clone(), edges_, dilate_kernel);



  canny_ = edges_.clone();

}

void BlockSeparator::FindPromisingAreas() {
  markup_ = original_.clone();
  for (int i = 0; i < groups_.rows; i++)
    for (int j = 0; j < groups_.cols; j++)
      if (!IsWhite(groups_, i, j))
        visited_[i][j] = true;


  Log::block_separator("Exploring objects");
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


        if (width/height > 1.7) {
          Log::block_separator("  removed ratio = %f", width/height);
          continue;
        }

        AddBlock(traverser.RowRange(), traverser.ColRange(), traverser.points());
      }

  ImageHandler::Save("markup", markup_);
}

std::vector<std::pair<cv::Mat, cv::Mat> > BlockSeparator::ExtractSubBlocks(const cv::Mat& input) {
  std::vector<std::pair<cv::Mat, cv::Mat> > blocks;

  cv::Mat block = input.clone();
  cv::cvtColor(block, block, CV_BGR2HSV);

  std::vector<cv::Mat> channels;

  for (int i = 0; i < 5; i++) {
    cv::Mat blurred;
    cv::GaussianBlur(block, blurred, cv::Size(0, 0), 5);
    cv::addWeighted(block, 1.5, blurred, -0.5, 0, block);
  }
  cv::split(block, channels);

  cv::Mat block_mask = cv::Mat(block.rows, block.cols, CV_8UC1);

  cv::inRange(block, cv::Scalar(0, 0, 0), cv::Scalar(255, 100, 120), block_mask);
  cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,5), cv::Point(3,2));
  for (int i =0; i < 6; i++)
    cv::dilate(block_mask, block_mask, dilate_kernel);

  cv::Mat block_rgb;
  cv::cvtColor(block, block_rgb, CV_HSV2BGR);

  std::vector<std::vector<cv::Point> > contours, number_contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(block_mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  Log::block_separator("  detected %ul subcontours", contours.size());

  std::vector<double> areas;
  double area_total = block.rows * block.cols;

  for (auto contour : contours)
    areas.push_back(cv::contourArea(contour));

  cv::cvtColor(block, block, CV_HSV2BGR);

  int i = 0;
  for (auto contour : contours) {
    if (areas[i++]/area_total < 0.05) {
      Log::block_separator("    removed fill ratio = %f", areas[i-1]/area_total);
      continue;
    }
    cv::Mat inner_block = cv::Mat::zeros(block.rows, block.cols, CV_8UC1);
    cv::Mat inner_mask = cv::Mat::zeros(block.rows, block.cols, CV_8UC1);
    cv::drawContours(inner_mask, std::vector<std::vector<cv::Point> > (1, contour), -1, cv::Scalar(255), CV_FILLED);


    inner_block.setTo(255);
    channels[2].copyTo(inner_block, inner_mask);

    cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3), cv::Point(1,1));
    //cv::erode(inner_block, inner_block, dilate_kernel);
    //cv::dilate(inner_block, inner_block, dilate_kernel);

    cv::threshold(inner_block, inner_block, 180, 255, 0);

    blocks.push_back(std::make_pair(inner_block, input));
  }
  return blocks;
}


int bind = 0;

void BlockSeparator::ApplyZurekFilter(std::pair<cv::Mat, cv::Mat>& image, const cv::Range& rows, const cv::Range& cols, const std::vector<std::pair<int, int> >& points) {
  ImageHandler::Save("zurekpre" + std::to_string(bind), image.first);
  using std::vector;

  int rbegin = rows.start;
  int cbegin = cols.start;

  // Mask image
  cv::Mat mask = image.first.clone();
  cv::cvtColor(mask, mask, CV_GRAY2BGR);


  // Mark all points white
  for (int i = 0; i < mask.rows; i++)
    for (int j = 0; j < mask.cols; j++)
      SetColor(&mask, i, j, 255, 255, 255);

  // Create copy of white mask
  cv::Mat mask_copy = mask.clone();

  // Mark all points from current area as black
  for (auto point : points)
    SetColor(&mask, point.first - rbegin, point.second - cbegin, 0, 0, 0);

  ImageHandler::Save("zurekmaskcopy" + std::to_string(bind), mask);
  // Erode to make sure that nothing dark touches the number
  cv::Mat erode_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9,9), cv::Point(4,4));
  cv::erode(mask.clone(), mask, erode_kernel);

  // Run outer dfs
  vector<bool> row(mask.cols, false);
  vector<vector<bool> > visited(mask.rows, row);

  for (int i = 0; i < mask.rows; i++)
    for (int j = 0; j < mask.cols; j++)
      if (!IsWhite(mask, i, j))
        visited[i][j] = true;

  for (int i = 0; i < mask.rows; i++)
    for (int j = 0; j < mask.cols; j++){
      if (!(i == 0 || i == mask.rows - 1 || j == 0 || j == mask.cols - 1)) continue;
      if (!visited[i][j]) {
        Traverser traverser(mask, mask, &visited);
        traverser.Run(i, j);
        for (auto point : traverser.points())
          SetColor(&mask_copy, point.first, point.second, 0, 0, 0);
      }
    }

  // Run eode again, this time it shrinks 'selected' area
  cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9,9), cv::Point(4,4));
  cv::erode(mask_copy, mask_copy, dilate_kernel);

  // Use mask to hide some pixels from processed block
  for (int i = 0; i < mask_copy.rows; i++)
    for (int j = 0; j < mask_copy.cols; j++)
      if (!IsWhite(mask_copy, i, j))
        SetColor(&image.first, i, j, 255, 255, 255);

  ImageHandler::Save("zurekmask" + std::to_string(bind++), mask_copy);

  Log::block_separator("Done Zurek filter");
}

void BlockSeparator::AddBlock(const cv::Range& rows, const cv::Range& cols, const std::vector<std::pair<int, int> >& points) {
  if (points.size() < kMinBlockSize)
    return;

  std::vector<std::pair<cv::Mat, cv::Mat> > b = ExtractSubBlocks(cv::Mat(sharp_, rows, cols).clone());

  for (int i = 0; i < b.size(); i++) {
    blocks_.push_back(std::make_pair(b[i].first.clone(), b[i].second.clone()));
    ApplyZurekFilter(b[i], rows, cols, points);
    blocks_.push_back(b[i]);

    Log::block_separator("Added block");
  }

  //blocks_.insert(blocks_.begin(), b.begin(), b.end());


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
