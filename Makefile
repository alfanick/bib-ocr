SOURCES=main.cpp Extractor.cpp

CXX=clang++ -Wall -std=c++11 -stdlib=libc++ -O3

OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))
.PHONY: clean

all: ocr

ocr: $(OBJECTS)
	$(CXX) $(OBJECTS) -o bin/ocr

obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@

clean:
	rm bin/ocr
	rm obj/*.o
