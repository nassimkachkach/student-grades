CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

APP_SRCS := main.cpp Student.cpp StudentFileReader.cpp StudentFileWriter.cpp RandomStudentGenerator.cpp Sorting.cpp StudentFileGenerator.cpp
GEN_SRCS := GenerateStudentFiles.cpp StudentFileGenerator.cpp Student.cpp RandomStudentGenerator.cpp

APP_BIN := build/main
GEN_BIN := build/generate_student_files

.PHONY: all clean run generate

all: $(APP_BIN) $(GEN_BIN)

build:
	mkdir -p build

$(APP_BIN): $(APP_SRCS) | build
	$(CXX) $(CXXFLAGS) $(APP_SRCS) -o $(APP_BIN)

$(GEN_BIN): $(GEN_SRCS) | build
	$(CXX) $(CXXFLAGS) $(GEN_SRCS) -o $(GEN_BIN)

run: $(APP_BIN)
	./$(APP_BIN)

generate: $(GEN_BIN)
	./$(GEN_BIN)

clean:
	rm -rf build
