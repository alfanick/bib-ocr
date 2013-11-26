#include "result.h"

namespace bib_ocr {

Result::Result() : number_(-1), probability_(-1), probabilities_() {
}

Result::Result(int number, int probability)
  : number_(number), probability_(probability), probabilities_() {
  probabilities_.push_back(probability);
}

Result::~Result() {
}

void Result::AddOccurence(int probability) {
  probabilities_.push_back(probability);
}

int Result::AverageProbability() {
  // TODO
  if (probabilities_.size() == 0) return 0;

  sort(probabilities_.begin(), probabilities_.end(), [] (int a, int b) { return a > b; });
  return probabilities_[0] + double(std::min((int) probabilities_.size(), 20) * probabilities_[0]) / 200.0;

  int sum = 0;
  for (int prob : probabilities_)
    sum += prob;

  return sum / probabilities_.size();
}

std::vector<int> Result::probabilities() {
  sort(probabilities_.begin(), probabilities_.end(), [] (int a, int b) { return a > b; });
  return probabilities_;
}

}  // namespace bib_ocr
