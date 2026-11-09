#ifndef STUDENT_FILE_WRITER_H
#define STUDENT_FILE_WRITER_H

#include "Student.h"

#include <cstddef>
#include <deque>
#include <filesystem>
#include <list>
#include <vector>

class StudentFileWriter {
public:
	static void writeStudentsToFile(const std::filesystem::path& filePath, const std::vector<Student>& students, std::size_t homeworkCount);
	static void writeStudentsToFile(const std::filesystem::path& filePath, const std::list<Student>& students, std::size_t homeworkCount);
	static void writeStudentsToFile(const std::filesystem::path& filePath, const std::deque<Student>& students, std::size_t homeworkCount);
};

#endif