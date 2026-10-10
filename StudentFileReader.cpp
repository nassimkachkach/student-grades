#include "StudentFileReader.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

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

bool StudentFileReader::readStudentsFromFile(const std::string& filePath, std::vector<Student>& students) {
	std::ifstream input(filePath);
	if (!input) {
		std::cerr << "Error: unable to open file '" << filePath << "'.\n";
		return false;
	}

	std::string line;
	std::size_t lineNumber = 0;
	std::size_t expectedHomeworkCount = 0;
	bool headerFound = false;

	while (std::getline(input, line)) {
		line = stripCarriageReturn(line);
		++lineNumber;

		if (line.empty()) {
			continue;
		}

		const std::vector<std::string> columns = splitWhitespace(line);
		if (!headerFound) {
			if (columns.size() < 3) {
				std::cerr << "Error: malformed header on line " << lineNumber << ": expected at least 3 columns.\n";
				return false;
			}

			if (columns[0] != "Vardas" || columns[1] != "Pavarde") {
				std::cerr << "Error: expected a header with 'Vardas Pavarde ... Egz.' on line " << lineNumber << ".\n";
				return false;
			}

			expectedHomeworkCount = columns.size() - 3;
			if (expectedHomeworkCount == 0) {
				std::cerr << "Error: no homework columns detected in header on line " << lineNumber << ".\n";
				return false;
			}

			headerFound = true;
			continue;
		}

		if (columns.size() != expectedHomeworkCount + 3) {
			std::cerr << "Error: malformed row on line " << lineNumber << ": expected "
					  << (expectedHomeworkCount + 3) << " columns, found " << columns.size() << ".\n";
			continue;
		}

		std::vector<int> homeworkScores;
		homeworkScores.reserve(expectedHomeworkCount);
		bool validRow = true;

		for (std::size_t i = 2; i + 1 < columns.size(); ++i) {
			int score = 0;
			if (!parseIntToken(columns[i], score) || score < 1 || score > 10) {
				std::cerr << "Error: malformed row on line " << lineNumber << ": invalid homework score '"
						  << columns[i] << "'.\n";
				validRow = false;
				break;
			}
			homeworkScores.push_back(score);
		}

		if (!validRow) {
			continue;
		}

		int examScore = 0;
		if (!parseIntToken(columns.back(), examScore) || examScore < 0 || examScore > 10) {
			std::cerr << "Error: malformed row on line " << lineNumber << ": invalid exam score '"
					  << columns.back() << "'.\n";
			continue;
		}

		Student student(columns[0], columns[1], homeworkScores, examScore);
		student.calculateFinalGrades();
		students.push_back(student);
	}

	if (!headerFound) {
		std::cerr << "Error: file '" << filePath << "' does not contain a valid header.\n";
		return false;
	}

	return true;
}