#ifndef STUDENT_FILE_READER_H
#define STUDENT_FILE_READER_H

#include "Student.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

class StudentRecordParseError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class StudentFileReader {
public:
	static std::filesystem::path resolveInputPath(const std::string& inputText);
	static void readStudentsFromFile(const std::string& filePath, std::vector<Student>& students);
};

#endif