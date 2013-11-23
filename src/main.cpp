#include <cstdlib>
#include <cstdio>

#include "Extractor.h"

using namespace BibOcr;

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("At least one image path is required!\n");

    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i++) {
    printf("Processing image '%s'\n", argv[i]);

    std::unique_ptr<Extractor> extractor(new Extractor(argv[i]));

    extractor->extract();

    printf("Done\n");
  }

  return EXIT_SUCCESS;
}
