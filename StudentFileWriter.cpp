#include "StudentFileWriter.h"

#include <fstream>
#include <stdexcept>

template <typename Container>
void StudentFileWriter::writeStudentsToFile(const std::filesystem::path& filePath, const Container& students, std::size_t homeworkCount) {
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

template void StudentFileWriter::writeStudentsToFile<std::vector<Student>>(const std::filesystem::path&, const std::vector<Student>&, std::size_t);
template void StudentFileWriter::writeStudentsToFile<std::list<Student>>(const std::filesystem::path&, const std::list<Student>&, std::size_t);
template void StudentFileWriter::writeStudentsToFile<std::deque<Student>>(const std::filesystem::path&, const std::deque<Student>&, std::size_t);