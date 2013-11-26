#ifndef RESULT_H_
#define RESULT_H_

#include <vector>

namespace bib_ocr {

// TODO(kareth) Later on, this class should allow setting
// probabilities for different digits of a number
class Result {
 public:
  Result();
  Result(int number, int probability);
  ~Result();

  void AddOccurence(int probability);

  int number() const { return number_; }
  int probability() { return AverageProbability(); }
  std::vector<int> probabilities();


  void set_number(int number) { number_ = number; }
  void set_probability(int probability) { probability_ = probability; }

 private:
  int AverageProbability();

  int number_;
  int probability_;
  std::vector<int> probabilities_;
};

}  // namespace bib_ocr

#endif  // RESULT_H_
