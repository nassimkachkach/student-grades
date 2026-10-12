#include "StudentFileReader.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
	std::string stripCarriageReturn(std::string line) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		return line;
	}

	std::vector<std::string> splitWhitespace(const std::string& text) {
		std::istringstream stream(text);
		std::vector<std::string> tokens;
		std::string token;

		while (stream >> token) {
			tokens.push_back(token);
		}

		return tokens;
	}

	bool parseIntToken(const std::string& token, int& value) {
		std::istringstream stream(token);
		if (!(stream >> value)) {
			return false;
		}

		stream >> std::ws;
		return stream.eof();
	}
}

std::filesystem::path StudentFileReader::resolveInputPath(const std::string& inputText) {
	std::filesystem::path candidate(inputText);
	if (candidate.empty()) {
		candidate = "students10000.txt";
	}

	if (candidate.is_absolute()) {
		return candidate;
	}

	if (std::filesystem::exists(candidate)) {
		return candidate;
	}

	const std::filesystem::path workspaceCandidate = std::filesystem::current_path() / candidate;
	if (std::filesystem::exists(workspaceCandidate)) {
		return workspaceCandidate;
	}

	const std::filesystem::path downloadsCandidate = std::filesystem::path("C:/Users/User/Downloads") / candidate;
	if (std::filesystem::exists(downloadsCandidate)) {
		return downloadsCandidate;
	}

	return candidate;
}

void StudentFileReader::readStudentsFromFile(const std::string& filePath, std::vector<Student>& students) {
	std::ifstream input(filePath);
	if (!input) {
		throw std::runtime_error("Unable to open file '" + filePath + "'.");
	}

	std::string line;
	std::size_t lineNumber = 0;
	std::size_t expectedHomeworkCount = 0;
	bool headerFound = false;
	std::vector<Student> loadedStudents;

	while (std::getline(input, line)) {
		line = stripCarriageReturn(line);
		++lineNumber;

		if (line.empty()) {
			continue;
		}

		const std::vector<std::string> columns = splitWhitespace(line);
		if (!headerFound) {
			if (columns.size() < 3) {
				throw StudentRecordParseError(
					"Malformed header on line " + std::to_string(lineNumber) + ": expected at least 3 columns.");
			}

			if (columns.at(0) != "Vardas" || columns.at(1) != "Pavarde") {
				throw StudentRecordParseError(
					"Expected a header with 'Vardas Pavarde ... Egz.' on line " + std::to_string(lineNumber) + ".");
			}

			expectedHomeworkCount = columns.size() - 3;
			if (expectedHomeworkCount == 0) {
				throw StudentRecordParseError(
					"No homework columns detected in header on line " + std::to_string(lineNumber) + ".");
			}

			headerFound = true;
			continue;
		}

		if (columns.size() != expectedHomeworkCount + 3) {
			throw StudentRecordParseError(
				"Malformed row on line " + std::to_string(lineNumber) + ": expected " +
				std::to_string(expectedHomeworkCount + 3) + " columns, found " + std::to_string(columns.size()) + ".");
		}

		std::vector<int> homeworkScores;
		homeworkScores.reserve(expectedHomeworkCount);

		for (std::size_t i = 2; i + 1 < columns.size(); ++i) {
			int score = 0;
			if (!parseIntToken(columns.at(i), score)) {
				throw StudentRecordParseError(
					"Malformed homework score on line " + std::to_string(lineNumber) + ": '" + columns.at(i) + "'.");
			}

			if (score < 1 || score > 10) {
				throw std::out_of_range(
					"Homework score out of range on line " + std::to_string(lineNumber) + ": '" + columns.at(i) + "'.");
			}

			homeworkScores.push_back(score);
		}

		int examScore = 0;
		if (!parseIntToken(columns.back(), examScore)) {
			throw StudentRecordParseError(
				"Malformed exam score on line " + std::to_string(lineNumber) + ": '" + columns.back() + "'.");
		}

		if (examScore < 0 || examScore > 10) {
			throw std::out_of_range(
				"Exam score out of range on line " + std::to_string(lineNumber) + ": '" + columns.back() + "'.");
		}

		Student student(columns.at(0), columns.at(1), homeworkScores, examScore);
		student.calculateFinalGrades();
		loadedStudents.push_back(student);
	}

	if (!headerFound) {
		throw StudentRecordParseError("File '" + filePath + "' does not contain a valid header.");
	}

	students = std::move(loadedStudents);
}