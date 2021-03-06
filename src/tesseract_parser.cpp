#include "tesseract_parser.h"

namespace bib_ocr {

TesseractParser::TesseractParser()
  : results_(), tess_() {
  tess_.Init(NULL, "eng");
  tess_.SetVariable("classify_enable_learning", "0");
  tess_.SetVariable("classify_enable_adaptive_matcher", "0");
}

TesseractParser::~TesseractParser() {
}

void TesseractParser::AddResult(int number, int probability) {
  for (int i = 0; i < results_.size(); i++){
    if (results_[i].number() == number) {
      results_[i].AddOccurence(probability);
      return;
    }
  }

  results_.push_back(Result(number, probability));
}

int TesseractParser::ProcessWord(const std::string& word, int probability) {
  //std::cout << " (prob: " << probability << ") >>" << word << "<<" << std::endl;
  try {
    int number = std::stoi(word);
    AddResult(number, probability);
    return 1;
  }
  catch (...) {
    return 0;
  }
}

int TesseractParser::ParseWords(char* words, int* probabilities) {
  int found = 0;
  std::string word;
  while (true) {
    if (*probabilities == -1) break;

    word = "";
    while (*words != 32 && *words != 10) { // space or end
      word += *words;
      words++;
    }

    found += ProcessWord(word, *probabilities);
    probabilities++;
    words++;
  }
  if (found > 0)
    return 0;
  else
    return -1;
}

int TesseractParser::Parse(const cv::Mat& original) {
  // TODO(kareth) this instance probably should be created once
  //tess.SetVariable("tessedit_char_whitelist", "0123456789");
  cv::Mat image = original.clone();

  tess_.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
  tess_.Recognize(0);
  char* words = tess_.GetUTF8Text();
  int* probabilities = tess_.AllWordConfidences();

  int success = ParseWords(words, probabilities);
  return success;
}

void TesseractParser::FilterResults() {
  //int max_score[] = {0, 100, 300, 600, 1000, 1500};
  int max_score[] = {0, 100, 300, 500, 700, 1500};

  using std::swap;
  std::sort(results_.begin(), results_.end(), [] (Result& a, Result& b) { return a.number() > b.number(); });

  std::vector<std::pair<int, int> > coverage;

  for (int i = 0; i < results_.size(); i++) {
    std::string num = std::to_string(results_[i].number());

    std::vector<int> scores(num.size(), 0);

    for(int j = i+1; j < results_.size(); j++) {
      std::string num2 = std::to_string(results_[j].number());
      std::size_t found = num.find(num2);
      while (found != std::string::npos) {
        for(int k = found; k < found + num2.size(); k++)
          scores[k] += results_[j].probability() / num2.size();
        found = num.find(num2, found + 1);
      }
    }
    int res = results_[i].probability();
    for (int score : scores) {
      res += score;
    }
    coverage.push_back(std::pair<int, int>(i, res));
  }
  sort(coverage.begin(), coverage.end(), [] (std::pair<int, int> a, std::pair<int, int> b) { return a.second > b.second; });

  for (auto p : coverage) {
    double probability = double(p.second) / double(max_score[std::to_string(results_[p.first].number()).size()]);
    Log::tesseract_parser("Filtered %d (%d) [%lf]", results_[p.first].number(), p.second, probability);
  }

  std::vector<Result> new_result;

  for (int treshold = 70; treshold > 40; treshold -= 10) {
    for (auto p : coverage) {

      int found = 0;
      for (auto ex : new_result)
        if(std::to_string(ex.number()).find(std::to_string(results_[p.first].number())) != std::string::npos)
          found = 1;
      if (found == 1)
        continue;

      int digits = std::to_string(results_[p.first].number()).size();
      if (digits == 1) continue;
      double probability = double(p.second) / double(max_score[digits]) * 100.0;
      if (probability > treshold || (probability > treshold - 10 && digits >= 3)) {
        new_result.push_back(Result(results_[p.first].number(), probability));
      }
    }
    if (new_result.size() > 0) break;
  }
  results_ = new_result;
}

std::vector<Result> TesseractParser::GetResult() {
  std::string pr = "";
  for (auto res : results_) {
    for (auto prob : res.probabilities())
      pr += std::to_string(prob) + " ";
    Log::tesseract_parser("Found %d {%d} (%s)",res.number(), res.probability(), pr.c_str());
  }

  FilterResults();

  // TODO get all good
  return results_;
}

}  // namespace bib_ocr
