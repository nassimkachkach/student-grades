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

	friend std::istream& operator>>(std::istream& in, Student& student);
	friend std::ostream& operator<<(std::ostream& out, const Student& student);

private:
	std::string firstName_;
	std::string surname_;
	std::vector<int> homeworkResults_;
	int examResult_;
	double finalGrade_;
};

#endif