#ifndef STUDENT_H
#define STUDENT_H

#include "lib.h"

#include <cstddef>
#include <string>
#include <vector>

class Student {
public:
	Student();
	explicit Student(std::size_t homeworkCount);
	Student(const Student& other);
	Student& operator=(const Student& other);
	~Student();

	void calculateFinalGrade();

	friend std::istream& operator>>(std::istream& in, Student& student);
	friend std::ostream& operator<<(std::ostream& out, const Student& student);

private:
	std::string firstName_;
	std::string surname_;
	std::vector<double> homeworkResults_;
	double examResult_;
	double finalGrade_;
};

#endif