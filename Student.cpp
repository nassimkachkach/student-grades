#include "Student.h"

#include <iomanip>

Student::Student()
	: firstName_(),
	  surname_(),
	  homeworkResults_(),
	  examResult_(0.0),
	  finalGrade_(0.0) {
}

Student::Student(std::size_t homeworkCount)
	: firstName_(),
	  surname_(),
	  homeworkResults_(homeworkCount, 0.0),
	  examResult_(0.0),
	  finalGrade_(0.0) {
}

Student::Student(const Student& other)
	: firstName_(other.firstName_),
	  surname_(other.surname_),
	  homeworkResults_(other.homeworkResults_),
	  examResult_(other.examResult_),
	  finalGrade_(other.finalGrade_) {
}

Student& Student::operator=(const Student& other) {
	if (this != &other) {
		firstName_ = other.firstName_;
		surname_ = other.surname_;
		homeworkResults_ = other.homeworkResults_;
		examResult_ = other.examResult_;
		finalGrade_ = other.finalGrade_;
	}

	return *this;
}

Student::~Student() {
}

void Student::calculateFinalGrade() {
	double homeworkAverage = 0.0;

	if (!homeworkResults_.empty()) {
		double homeworkSum = 0.0;

		for (double homeworkResult : homeworkResults_) {
			homeworkSum += homeworkResult;
		}

		homeworkAverage = homeworkSum / static_cast<double>(homeworkResults_.size());
	}

	finalGrade_ = 0.4 * homeworkAverage + 0.6 * examResult_;
}

std::istream& operator>>(std::istream& in, Student& student) {
	in >> student.firstName_ >> student.surname_;

	for (double& homeworkResult : student.homeworkResults_) {
		in >> homeworkResult;
	}

	in >> student.examResult_;

	if (in) {
		student.calculateFinalGrade();
	}

	return in;
}

std::ostream& operator<<(std::ostream& out, const Student& student) {
	out << std::left << std::setw(12) << student.firstName_
		<< std::left << std::setw(15) << student.surname_
		<< std::right << std::fixed << std::setprecision(2) << std::setw(20) << student.finalGrade_;

	return out;
}