#include "lib.h"
#include "Student.h"

#include <iomanip>
#include <string>
#include <vector>

namespace {
	Student::GradeMethod readGradeMethod() {
		std::string method;

		while (true) {
			std::cout << "Choose final grade method ([A]verage/[M]edian): ";
			if (!std::getline(std::cin >> std::ws, method)) {
				return Student::GradeMethod::Average;
			}

			if (method == "A" || method == "a" || method == "Average" || method == "average") {
				return Student::GradeMethod::Average;
			}

			if (method == "M" || method == "m" || method == "Median" || method == "median") {
				return Student::GradeMethod::Median;
			}

			std::cout << "Invalid choice. Enter A for average or M for median.\n";
		}
	}

	const char* headerForMethod(Student::GradeMethod method) {
		return method == Student::GradeMethod::Median ? "Final_Point(Med.)" : "Final_Point(Aver.)";
	}
}

int main() {
    std::size_t studentCount = 0;
    Student::GradeMethod gradeMethod = readGradeMethod();

    std::cout << "Enter number of students: ";
    std::cin >> studentCount;

    if (!std::cin) {
        std::cerr << "Invalid student count input.\n";
        return 1;
    }

    std::vector<Student> students;
    students.reserve(studentCount);

    for (std::size_t i = 0; i < studentCount; ++i) {
        std::cout << "Enter student " << (i + 1) << " name and surname: ";
        students.emplace_back();
        std::cin >> students.back();
        if (!std::cin) {
            std::cerr << "Invalid student input.\n";
            return 1;
        }

        students.back().calculateFinalGrade(gradeMethod);
    }

    std::cout << std::left << std::setw(12) << "Name"
              << std::left << std::setw(15) << "Surname"
              << std::right << std::setw(20) << headerForMethod(gradeMethod) << '\n';
    std::cout << std::string(47, '-') << '\n';

    for (const Student& student : students) {
        std::cout << student << '\n';
    }

    return 0;
}