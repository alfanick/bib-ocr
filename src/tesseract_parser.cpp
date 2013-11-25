#include "tesseract_parser.h"

namespace bib_ocr {

TesseractParser::TesseractParser(cv::Mat* image)
  : image_(image), results_() {
}

TesseractParser::~TesseractParser() {
}

int TesseractParser::ProcessWord(const std::string& word, int probability) {
  std::cout << " (prob: " << probability << ") >>" << word << "<<" << std::endl;
  try {
    int number = std::stoi(word);
    results_.push_back(Result(number, probability));
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


int TesseractParser::Parse() {
  // TODO(kareth) this instance probably should be created once
  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng");
  //tess.SetVariable("classify_bln_numeric_mode", "1");
  //tess.SetVariable("tessedit_char_whitelist", "0123456789");
  tess.SetImage((uchar*)image_->data, image_->size().width, image_->size().height, image_->channels(), image_->step1());
  tess.Recognize(0);
  char* words = tess.GetUTF8Text();
  int* probabilities = tess.AllWordConfidences();

  int success = ParseWords(words, probabilities);
  //delete[] words;
  //delete[] probabilities;
  return success;
}

Result TesseractParser::GetResult() const {
  return results_[0];
}

}  // namespace bib_ocr
