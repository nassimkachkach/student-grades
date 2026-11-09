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
	// Instantiated for std::vector<Student>, std::list<Student> and std::deque<Student>.
	template <typename Container>
	static void writeStudentsToFile(const std::filesystem::path& filePath, const Container& students, std::size_t homeworkCount);
};

#endif