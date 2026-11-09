#ifndef STUDENT_FILE_READER_H
#define STUDENT_FILE_READER_H

#include "Student.h"

#include <deque>
#include <filesystem>
#include <list>
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

	// Instantiated for std::vector<Student>, std::list<Student> and std::deque<Student>.
	template <typename Container>
	static void readStudentsFromFile(const std::string& filePath, Container& students);
};

#endif