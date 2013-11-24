#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "extractor.h"

using namespace bib_ocr;

int main(int argc, char **argv) {
  srand(time(0));
  if (argc <= 1) {
    printf("At least one image path is required!\n");

    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i++) {
    printf("Processing image '%s'\n", argv[i]);

    std::unique_ptr<Extractor> extractor(new Extractor(argv[i]));
    extractor->Extract();

    for (auto result : extractor->GetNumbers())
      printf("(%d %d)\n", result.number(), result.probability());

    printf("Done\n");
  }

  return EXIT_SUCCESS;
}
