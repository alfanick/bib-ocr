#include "result.h"

namespace bib_ocr {

Result::Result() : number_(-1), probability_(-1) {
}

Result::Result(int number, int probability)
  : number_(number), probability_(probability) {
}

Result::~Result() {
}

}  // namespace bib_ocr
