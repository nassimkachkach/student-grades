#ifndef STUDENT_FILE_WRITER_H
#define STUDENT_FILE_WRITER_H

#include "Student.h"

#include <cstddef>
#include <filesystem>
#include <vector>

class StudentFileWriter {
public:
	static void writeStudentsToFile(const std::filesystem::path& filePath, const std::vector<Student>& students, std::size_t homeworkCount);
};

#endif