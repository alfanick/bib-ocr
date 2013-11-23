#include <cstdlib>
#include <cstdio>

#include "Extractor.h"

using namespace BibOcr;

int main(int argc, char **argv) {
  printf("Hello world!\n");

  std::unique_ptr<Extractor> extractor(new Extractor());

  return EXIT_SUCCESS;
}
