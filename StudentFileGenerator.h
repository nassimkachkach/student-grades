#ifndef STUDENT_FILE_GENERATOR_H
#define STUDENT_FILE_GENERATOR_H

#include <cstddef>
#include <filesystem>
#include <string>

class StudentFileGenerator {
public:
    static void generateStudentFile(const std::filesystem::path& filePath, std::size_t studentCount, std::size_t homeworkCount = 10);
};

#endif
