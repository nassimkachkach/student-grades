#include "Student.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>

namespace {
	constexpr int kMinScore = 1;
	constexpr int kMaxScore = 10;
	constexpr double kHomeworkWeight = 0.4;
	constexpr double kExamWeight = 0.6;

	bool isScoreInRange(int score) {
		return score >= kMinScore && score <= kMaxScore;
	}

	bool parseWholeLineAsInt(const std::string& line, int& value) {
		std::istringstream stream(line);

		if (!(stream >> value)) {
			return false;
		}

		stream >> std::ws;
		return stream.eof();
	}
}

std::mt19937& randomEngine() {
	static std::mt19937 engine{std::random_device{}()};
	return engine;
}

int generateRandomScore() {
	std::uniform_int_distribution<int> distribution(kMinScore, kMaxScore);
	return distribution(randomEngine());
}

std::size_t generateRandomCount(std::size_t minCount, std::size_t maxCount) {
	std::uniform_int_distribution<std::size_t> distribution(minCount, maxCount);
	return distribution(randomEngine());
}

Student::Student()
	: firstName_(),
	  surname_(),
	  homeworkResults_(),
	  examResult_(0),
	  finalGrade_(0.0),
	  averageFinalGrade_(0.0),
	  medianFinalGrade_(0.0) {
}

Student::Student(const std::string& firstName, const std::string& surname, const std::vector<int>& homeworkResults, int examResult)
	: firstName_(firstName),
	  surname_(surname),
	  homeworkResults_(homeworkResults),
	  examResult_(examResult),
	  finalGrade_(0.0),
	  averageFinalGrade_(0.0),
	  medianFinalGrade_(0.0) {
}

Student::Student(const Student& other)
	: firstName_(other.firstName_),
	  surname_(other.surname_),
	  homeworkResults_(other.homeworkResults_),
	  examResult_(other.examResult_),
	  finalGrade_(other.finalGrade_),
	  averageFinalGrade_(other.averageFinalGrade_),
	  medianFinalGrade_(other.medianFinalGrade_) {
}

Student& Student::operator=(const Student& other) {
	if (this != &other) {
		firstName_ = other.firstName_;
		surname_ = other.surname_;
		homeworkResults_ = other.homeworkResults_;
		examResult_ = other.examResult_;
		finalGrade_ = other.finalGrade_;
		averageFinalGrade_ = other.averageFinalGrade_;
		medianFinalGrade_ = other.medianFinalGrade_;
	}

	return *this;
}

Student::~Student() {
}

double calculateMedian(const std::vector<int>& values) {
	if (values.empty()) {
		return 0.0;
	}

	std::vector<int> sortedValues = values;
	std::sort(sortedValues.begin(), sortedValues.end());

	const std::size_t middle = sortedValues.size() / 2;

	if (sortedValues.size() % 2 == 0) {
		return static_cast<double>(sortedValues[middle - 1] + sortedValues[middle]) / 2.0;
	}

	return static_cast<double>(sortedValues[middle]);
}

void Student::calculateFinalGrades() {
	double homeworkAverage = 0.0;
	if (!homeworkResults_.empty()) {
		double homeworkSum = 0.0;
		for (int homeworkResult : homeworkResults_) {
			homeworkSum += homeworkResult;
		}
		homeworkAverage = homeworkSum / static_cast<double>(homeworkResults_.size());
	}

	const double homeworkMedian = calculateMedian(homeworkResults_);
	averageFinalGrade_ = kHomeworkWeight * homeworkAverage + kExamWeight * static_cast<double>(examResult_);
	medianFinalGrade_ = kHomeworkWeight * homeworkMedian + kExamWeight * static_cast<double>(examResult_);
	finalGrade_ = averageFinalGrade_;
}

void Student::calculateFinalGrade(GradeMethod method) {
	double homeworkAverage = 0.0;
	double homeworkValue = 0.0;

	if (!homeworkResults_.empty()) {
		double homeworkSum = 0.0;
		for (int homeworkResult : homeworkResults_) {
			homeworkSum += homeworkResult;
		}
		homeworkAverage = homeworkSum / static_cast<double>(homeworkResults_.size());
	}

	if (method == GradeMethod::Median) {
		homeworkValue = calculateMedian(homeworkResults_);
	} else {
		homeworkValue = homeworkAverage;
	}

	finalGrade_ = kHomeworkWeight * homeworkValue + kExamWeight * static_cast<double>(examResult_);
	averageFinalGrade_ = kHomeworkWeight * homeworkAverage + kExamWeight * static_cast<double>(examResult_);
	medianFinalGrade_ = kHomeworkWeight * calculateMedian(homeworkResults_) + kExamWeight * static_cast<double>(examResult_);
}

const std::string& Student::firstName() const {
	return firstName_;
}

const std::string& Student::surname() const {
	return surname_;
}

const std::vector<int>& Student::homeworkResults() const {
	return homeworkResults_;
}

int Student::examResult() const {
	return examResult_;
}

double Student::averageFinalGrade() const {
	return averageFinalGrade_;
}

double Student::medianFinalGrade() const {
	return medianFinalGrade_;
}

double Student::finalGrade() const {
	return finalGrade_;
}

std::istream& operator>>(std::istream& in, Student& student) {
	in >> student.firstName_ >> student.surname_;
	if (!in) {
		return in;
	}
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	student.homeworkResults_.clear();

	std::string line;

	while (true) {
		if (&in == &std::cin) {
			std::cout << "Enter homework score [1-10] (negative or empty line to finish): ";
		}

		if (!std::getline(in, line)) {
			return in;
		}

		if (line.empty()) {
			break;
		}

		int score = 0;
		if (!parseWholeLineAsInt(line, score)) {
			if (&in == &std::cin) {
				std::cout << "Invalid input. Please enter a whole number.\n";
				continue;
			}

			in.setstate(std::ios::failbit);
			return in;
		}

		if (score < 0) {
			break;
		}

		if (!isScoreInRange(score)) {
			if (&in == &std::cin) {
				std::cout << "Homework score must be in range [1-10].\n";
				continue;
			}

			in.setstate(std::ios::failbit);
			return in;
		}

		student.homeworkResults_.push_back(score);
	}

	while (true) {
		if (&in == &std::cin) {
			std::cout << "Enter exam score [1-10]: ";
		}

		if (!std::getline(in, line)) {
			return in;
		}

		if (line.empty()) {
			if (&in == &std::cin) {
				std::cout << "Exam score is required.\n";
				continue;
			}

			in.setstate(std::ios::failbit);
			return in;
		}

		int examScore = 0;
		if (!parseWholeLineAsInt(line, examScore) || !isScoreInRange(examScore)) {
			if (&in == &std::cin) {
				std::cout << "Exam score must be a whole number in range [1-10].\n";
				continue;
			}

			in.setstate(std::ios::failbit);
			return in;
		}

		student.examResult_ = examScore;
		break;
	}

	return in;
}

std::ostream& operator<<(std::ostream& out, const Student& student) {
	out << std::left << std::setw(12) << student.firstName_
		<< std::left << std::setw(15) << student.surname_
		<< std::right << std::fixed << std::setprecision(2) << std::setw(20) << student.finalGrade_;

	return out;
}