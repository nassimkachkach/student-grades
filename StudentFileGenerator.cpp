#include "StudentFileGenerator.h"

#include "RandomStudentGenerator.h"

#include <fstream>
#include <stdexcept>
#include <vector>

namespace {
    std::vector<int> makeRandomHomeworkScores(std::size_t homeworkCount) {
        std::vector<int> scores;
        scores.reserve(homeworkCount);

        for (std::size_t i = 0; i < homeworkCount; ++i) {
            scores.push_back(RandomStudentGenerator::generateRandomScore());
        }

        return scores;
    }
}

void StudentFileGenerator::generateStudentFile(const std::filesystem::path& filePath, std::size_t studentCount, std::size_t homeworkCount) {
    if (studentCount == 0) {
        throw std::invalid_argument("Student count must be greater than zero.");
    }

    if (homeworkCount == 0) {
        throw std::invalid_argument("Homework score count must be greater than zero.");
    }

    const std::filesystem::path parentPath = filePath.parent_path();
    if (!parentPath.empty()) {
        std::filesystem::create_directories(parentPath);
    }

    std::ofstream output(filePath);
    if (!output) {
        throw std::runtime_error("Unable to open file for writing: '" + filePath.string() + "'.");
    }

    output << "Vardas Pavarde";
    for (std::size_t i = 1; i <= homeworkCount; ++i) {
        output << " ND" << i;
    }
    output << " Egz.\n";

    for (std::size_t i = 1; i <= studentCount; ++i) {
        const std::string firstName = "Name" + std::to_string(i);
        const std::string surname = "Surname" + std::to_string(i);
        const std::vector<int> homeworkScores = makeRandomHomeworkScores(homeworkCount);
        const int examScore = RandomStudentGenerator::generateRandomScore();

        output << firstName << ' ' << surname;
        for (int score : homeworkScores) {
            output << ' ' << score;
        }
        output << ' ' << examScore << '\n';
    }

    output.flush();
    if (!output) {
        throw std::runtime_error("Failed while writing file: '" + filePath.string() + "'.");
    }
}
