#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "logger.h"
#include "extractor.h"

using namespace bib_ocr;

int main(int argc, char **argv) {
  //Log::file("foo.log");
  Log::info("siema");
  srand(time(0));
  if (argc <= 1) {
    printf("At least one image or dataset path is required!\n");

    return EXIT_FAILURE;
  }

  if (argc == 2) {
    if (!(strlen(argv[1]) > 4 && !strcmp(argv[1] + strlen(argv[1]) - 4, ".jpg"))) {
      // read
      FILE *f = fopen(argv[1], "r+");
      if (f == NULL) {
        printf("Config file does not exist.\n");
        return EXIT_FAILURE;
      }

      char filename[255];
      int bsh, bss, bsv, beh, bes, bev, wsh, wss, wsv, weh, wes, wev;
      int c;

      while ((c = fscanf(f, "%s %d %d %d %d %d %d %d %d %d %d %d %d", filename,
            &bsh, &bss, &bsv, &beh, &bes, &bev, &wsh, &wss, &wsv, &weh, &wes, &wev)) > 0) {
        if (filename[0] == '#')
          continue;

        std::unique_ptr<Extractor> extractor;

        if (c == 1) {
          extractor = std::unique_ptr<Extractor>(new Extractor(filename));
        } else {
          extractor = std::unique_ptr<Extractor>(new Extractor(filename,
                cv::Scalar(bsh, bss, bsv),
                cv::Scalar(beh, bes, bev),
                cv::Scalar(wsh, wss, wsv),
                cv::Scalar(weh, wes, wev)));
        }

        extractor->Extract();
        for (auto result : extractor->GetNumbers())
          printf("(%d %d)\n", result.number(), result.probability());

        printf("Done\n");
      }

      fclose(f);
      return EXIT_SUCCESS;
    }
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
