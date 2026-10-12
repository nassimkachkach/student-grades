#include "StudentFileGenerator.h"

#include <filesystem>
#include <iostream>
#include <vector>

int main() {
    const std::vector<std::size_t> studentCounts = {1000, 10000, 100000, 1000000, 10000000};
    const std::filesystem::path outputDirectory = "generated";
    std::filesystem::create_directories(outputDirectory);

    for (std::size_t studentCount : studentCounts) {
        const std::filesystem::path filePath = outputDirectory / ("students_" + std::to_string(studentCount) + ".txt");
        StudentFileGenerator::generateStudentFile(filePath, studentCount, 10);
        std::cout << "Generated " << filePath << '\n';
    }

    return 0;
}
