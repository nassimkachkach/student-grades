#ifndef STUDENT_FILE_READER_H
#define STUDENT_FILE_READER_H

#include "Student.h"

#include <filesystem>
#include <string>
#include <vector>

class StudentFileReader {
public:
	static std::filesystem::path resolveInputPath(const std::string& inputText);
	static bool readStudentsFromFile(const std::string& filePath, std::vector<Student>& students);
};

#endif