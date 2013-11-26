SOURCES=main.cpp Extractor.cpp block_separator.cpp number_reader.cpp result.cpp image_handler.cpp traverser.cpp exif.cpp tesseract_parser.cpp promising_areas.cpp

CXX=clang++ -Wall -std=c++11 -stdlib=libc++ -O3 -I/usr/local/include

OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))
.PHONY: clean

all: bin/ocr
ocr: bin/ocr

bin/ocr: $(OBJECTS)
	$(CXX) $(OBJECTS) -o bin/ocr -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -ltesseract

obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@

clean:
	rm bin/ocr
	rm obj/*.o
