#ifndef STUDENT_H
#define STUDENT_H

#include "lib.h"

#include <cstddef>
#include <string>
#include <vector>

class Student {
public:
	enum class GradeMethod {
		Average,
		Median
	};

	Student();
	Student(const std::string& firstName, const std::string& surname, const std::vector<int>& homeworkResults, int examResult);
	Student(const Student& other);
	Student& operator=(const Student& other);
	~Student();

	void calculateFinalGrade(GradeMethod method);
	void calculateFinalGrades();

	const std::string& firstName() const;
	const std::string& surname() const;
	const std::vector<int>& homeworkResults() const;
	int examResult() const;
	double averageFinalGrade() const;
	double medianFinalGrade() const;
	double finalGrade() const;

	friend std::istream& operator>>(std::istream& in, Student& student);
	friend std::ostream& operator<<(std::ostream& out, const Student& student);

private:
	std::string firstName_;
	std::string surname_;
	std::vector<int> homeworkResults_;
	int examResult_;
	double finalGrade_;
	double averageFinalGrade_;
	double medianFinalGrade_;
};

#endif