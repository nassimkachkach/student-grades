#include "StudentFileWriter.h"

#include <fstream>
#include <stdexcept>

void StudentFileWriter::writeStudentsToFile(const std::filesystem::path& filePath, const std::vector<Student>& students, std::size_t homeworkCount) {
	if (homeworkCount == 0) {
		throw std::invalid_argument("Homework score count must be greater than zero.");
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

	for (const Student& student : students) {
		output << student.firstName() << ' ' << student.surname();
		for (int score : student.homeworkResults()) {
			output << ' ' << score;
		}
		output << ' ' << student.examResult() << '\n';
	}

	output.flush();
	if (!output) {
		throw std::runtime_error("Failed while writing file: '" + filePath.string() + "'.");
	}
}