#include "Sorting.h"

#include <cctype>

namespace {
	bool isDigit(char value) {
		return std::isdigit(static_cast<unsigned char>(value)) != 0;
	}

	bool compareNaturalStrings(const std::string& left, const std::string& right) {
	std::size_t leftIndex = 0;
	std::size_t rightIndex = 0;

	while (leftIndex < left.size() && rightIndex < right.size()) {
		if (isDigit(left[leftIndex]) && isDigit(right[rightIndex])) {
			const std::size_t leftNumberStart = leftIndex;
			const std::size_t rightNumberStart = rightIndex;

			while (leftIndex < left.size() && isDigit(left[leftIndex])) {
				++leftIndex;
			}
			while (rightIndex < right.size() && isDigit(right[rightIndex])) {
				++rightIndex;
			}

			const std::string leftNumber = left.substr(leftNumberStart, leftIndex - leftNumberStart);
			const std::string rightNumber = right.substr(rightNumberStart, rightIndex - rightNumberStart);

			if (leftNumber.size() != rightNumber.size()) {
				return leftNumber.size() < rightNumber.size();
			}

			if (leftNumber != rightNumber) {
				return leftNumber < rightNumber;
			}

			continue;
		}

		if (left[leftIndex] < right[rightIndex]) {
			return true;
		}

		if (left[leftIndex] > right[rightIndex]) {
			return false;
		}

		++leftIndex;
		++rightIndex;
	}

	return left.size() < right.size();
}

	bool compareStudentsByName(const Student& left, const Student& right) {
		if (left.firstName() == right.firstName()) {
			return compareNaturalStrings(left.surname(), right.surname());
		}
		return compareNaturalStrings(left.firstName(), right.firstName());
	}

	bool compareStudentsBySurname(const Student& left, const Student& right) {
		if (left.surname() == right.surname()) {
			return compareNaturalStrings(left.firstName(), right.firstName());
		}
		return compareNaturalStrings(left.surname(), right.surname());
	}
}

bool StudentComparator::compareByName(const Student& left, const Student& right) {
	return compareStudentsByName(left, right);
}

bool StudentComparator::compareBySurname(const Student& left, const Student& right) {
	return compareStudentsBySurname(left, right);
}