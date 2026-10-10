#include "RandomStudentGenerator.h"

#include <iostream>
#include <limits>

namespace {
	constexpr std::size_t kMinHomeworkCount = 1;
	constexpr std::size_t kMaxHomeworkCount = 10;
	constexpr int kMinScore = 1;
	constexpr int kMaxScore = 10;

	std::size_t askChoice(const std::string& prompt, std::size_t minimum, std::size_t maximum) {
		while (true) {
			std::size_t choice = 0;
			std::cout << prompt;

			if (std::cin >> choice && choice >= minimum && choice <= maximum) {
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return choice;
			}

			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Please enter a number from " << minimum << " to " << maximum << ".\n";
		}
	}

	std::string makeGeneratedName(const std::string& prefix, std::size_t index) {
		return prefix + std::to_string(index);
	}

	std::vector<int> makeRandomHomeworkScores(std::size_t homeworkCount) {
		std::vector<int> homeworkScores;
		homeworkScores.reserve(homeworkCount);

		for (std::size_t i = 0; i < homeworkCount; ++i) {
			homeworkScores.push_back(RandomStudentGenerator::generateRandomScore());
		}

		return homeworkScores;
	}

	void showGeneratedData(const std::string& firstName, const std::string& surname, const std::vector<int>& homeworkScores, int examScore) {
		std::cout << firstName << ' ' << surname << " -> homework:";
		for (int score : homeworkScores) {
			std::cout << ' ' << score;
		}
		std::cout << " | exam: " << examScore << '\n';
	}
}

std::mt19937& RandomStudentGenerator::randomEngine() {
	static std::random_device randomDevice;
	static std::mt19937 engine{randomDevice()};
	return engine;
}

int RandomStudentGenerator::generateRandomScore() {
	std::uniform_int_distribution<int> distribution(kMinScore, kMaxScore);
	return distribution(randomEngine());
}

std::size_t RandomStudentGenerator::generateRandomCount(std::size_t minCount, std::size_t maxCount) {
	std::uniform_int_distribution<std::size_t> distribution(minCount, maxCount);
	return distribution(randomEngine());
}

void RandomStudentGenerator::generateStudents(std::vector<Student>& students, std::size_t studentCount, Student::GradeMethod gradeMethod) {
	const std::size_t nameMode = askChoice("Choose name mode ([1] Typed / [2] Auto-generated): ", 1, 2);
	const std::size_t homeworkMode = askChoice("Choose homework count mode ([1] Fixed / [2] Random): ", 1, 2);

	std::size_t fixedHomeworkCount = 0;
	if (homeworkMode == static_cast<std::size_t>(HomeworkMode::Fixed)) {
		fixedHomeworkCount = askChoice("Enter homework count: ", kMinHomeworkCount, kMaxHomeworkCount);
	}

	for (std::size_t i = 0; i < studentCount; ++i) {
		std::string firstName;
		std::string surname;

		if (nameMode == static_cast<std::size_t>(NameMode::Typed)) {
			std::cout << "Enter student " << (i + 1) << " name and surname: ";
			std::cin >> firstName >> surname;

			if (!std::cin) {
				std::cerr << "Invalid student input.\n";
				return;
			}
		} else {
			firstName = makeGeneratedName("Name", i + 1);
			surname = makeGeneratedName("Surname", i + 1);
		}

		std::size_t homeworkCount = fixedHomeworkCount;
		if (homeworkMode == static_cast<std::size_t>(HomeworkMode::Random)) {
			homeworkCount = generateRandomCount(kMinHomeworkCount, kMaxHomeworkCount);
		}

		std::vector<int> homeworkScores = makeRandomHomeworkScores(homeworkCount);
		const int examScore = generateRandomScore();

		showGeneratedData(firstName, surname, homeworkScores, examScore);

		Student student(firstName, surname, homeworkScores, examScore);
		student.calculateFinalGrade(gradeMethod);
		students.push_back(student);
	}
}