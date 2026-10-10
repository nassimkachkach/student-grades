#ifndef SORTING_H
#define SORTING_H

#include "Student.h"

#include <string>

class StudentComparator {
public:
	static bool compareByName(const Student& left, const Student& right);
	static bool compareBySurname(const Student& left, const Student& right);
};

#endif