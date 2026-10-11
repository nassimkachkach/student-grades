#include "lib.h"
#include "Student.h"
#include "RandomStudentGenerator.h"
#include "Sorting.h"
#include "StudentFileReader.h"

#include <algorithm>
#include <exception>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

namespace {
    Student::GradeMethod askGradeMethod() {
        while (true) {
            std::string answer;
            std::cout << "Choose final grade method ([A]verage/[M]edian): ";

            if (!std::getline(std::cin >> std::ws, answer)) {
                return Student::GradeMethod::Average;
            }

            if (answer == "A" || answer == "a" || answer == "Average" || answer == "average") {
                return Student::GradeMethod::Average;
            }

            if (answer == "M" || answer == "m" || answer == "Median" || answer == "median") {
                return Student::GradeMethod::Median;
            }

            std::cout << "Please type A or M.\n";
        }
    }

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

    std::string askText(const std::string& prompt) {
        std::string value;
        std::cout << prompt;
        std::getline(std::cin, value);
        return value;
    }

    void printResults(const std::vector<Student>& students) {
        std::size_t nameWidth = std::string("Name").size();
        std::size_t surnameWidth = std::string("Surname").size();

        for (const Student& student : students) {
            nameWidth = std::max(nameWidth, student.firstName().size());
            surnameWidth = std::max(surnameWidth, student.surname().size());
        }

        const std::size_t valueWidth = 12;
        const std::string separator(nameWidth + surnameWidth + valueWidth * 2 + 14, '-');

        std::cout << std::left << std::setw(static_cast<int>(nameWidth)) << "Name"
                  << "  " << std::left << std::setw(static_cast<int>(surnameWidth)) << "Surname"
                  << "  " << std::right << std::setw(static_cast<int>(valueWidth)) << "Final (Avg.)"
                  << " | " << std::right << std::setw(static_cast<int>(valueWidth)) << "Final (Med.)" << '\n';
        std::cout << separator << '\n';

        for (const Student& student : students) {
            std::cout << std::left << std::setw(static_cast<int>(nameWidth)) << student.firstName()
                      << "  " << std::left << std::setw(static_cast<int>(surnameWidth)) << student.surname()
                      << "  " << std::fixed << std::setprecision(2) << std::right << std::setw(static_cast<int>(valueWidth))
                      << student.averageFinalGrade() << " | " << std::setw(static_cast<int>(valueWidth)) << student.medianFinalGrade() << '\n';
        }
    }
}

int main() {
    try {
        std::size_t inputMode = askChoice("Choose input mode ([1] Manual / [2] Random / [3] File): ", 1, 3);
        Student::GradeMethod gradeMethod = askGradeMethod();
        std::vector<Student> students;

        if (inputMode == 1) {
            std::size_t studentCount = askChoice("Enter number of students: ", 1, 1000);
            students.reserve(studentCount);
            for (std::size_t i = 0; i < studentCount; ++i) {
                Student student;
                std::cout << "Enter student " << (i + 1) << " name and surname: ";
                std::cin >> student;

                if (!std::cin) {
                    std::cerr << "Invalid student input.\n";
                    return 1;
                }

                student.calculateFinalGrade(gradeMethod);
                students.push_back(student);
            }
        } else if (inputMode == 2) {
            std::size_t studentCount = askChoice("Enter number of students: ", 1, 1000);
            students.reserve(studentCount);
            RandomStudentGenerator::generateStudents(students, studentCount, gradeMethod);
        } else {
            std::string filePath = askText("Enter file path (blank uses students10000.txt): ");
            const std::filesystem::path resolvedPath = StudentFileReader::resolveInputPath(filePath);
            StudentFileReader::readStudentsFromFile(resolvedPath.string(), students);
        }

        std::size_t sortChoice = askChoice("Sort by ([1] Name / [2] Surname): ", 1, 2);
        if (sortChoice == 1) {
            std::sort(students.begin(), students.end(), StudentComparator::compareByName);
        } else {
            std::sort(students.begin(), students.end(), StudentComparator::compareBySurname);
        }

        printResults(students);
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}