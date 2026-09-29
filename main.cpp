#include "lib.h"
#include "Student.h"

#include <iomanip>
#include <limits>
#include <string>
#include <vector>

namespace {
    constexpr std::size_t kMinHomeworkCount = 1;
    constexpr std::size_t kMaxHomeworkCount = 10;

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

    std::string makeGeneratedName(const std::string& prefix, std::size_t index) {
        return prefix + std::to_string(index);
    }

    std::vector<int> makeRandomHomeworkScores(std::size_t homeworkCount) {
        std::vector<int> homeworkScores;
        homeworkScores.reserve(homeworkCount);

        for (std::size_t i = 0; i < homeworkCount; ++i) {
            homeworkScores.push_back(generateRandomScore());
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

    void readManualStudents(std::vector<Student>& students, std::size_t studentCount, Student::GradeMethod gradeMethod) {
        for (std::size_t i = 0; i < studentCount; ++i) {
            Student student;
            std::cout << "Enter student " << (i + 1) << " name and surname: ";
            std::cin >> student;

            if (!std::cin) {
                std::cerr << "Invalid student input.\n";
                return;
            }

            student.calculateFinalGrade(gradeMethod);
            students.push_back(student);
        }
    }

    void readRandomStudents(std::vector<Student>& students, std::size_t studentCount, Student::GradeMethod gradeMethod) {
        std::size_t nameMode = askChoice("Choose name mode ([1] Typed / [2] Auto-generated): ", 1, 2);
        std::size_t homeworkMode = askChoice("Choose homework count mode ([1] Fixed / [2] Random): ", 1, 2);

        std::size_t fixedHomeworkCount = 0;
        if (homeworkMode == 1) {
            fixedHomeworkCount = askChoice("Enter homework count: ", kMinHomeworkCount, kMaxHomeworkCount);
        }

        for (std::size_t i = 0; i < studentCount; ++i) {
            std::string firstName;
            std::string surname;

            if (nameMode == 1) {
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
            if (homeworkMode == 2) {
                homeworkCount = generateRandomCount(kMinHomeworkCount, kMaxHomeworkCount);
            }

            std::vector<int> homeworkScores = makeRandomHomeworkScores(homeworkCount);
            int examScore = generateRandomScore();

            showGeneratedData(firstName, surname, homeworkScores, examScore);

            Student student(firstName, surname, homeworkScores, examScore);
            student.calculateFinalGrade(gradeMethod);
            students.push_back(student);
        }
    }

    void printResults(const std::vector<Student>& students, Student::GradeMethod gradeMethod) {
        const char* finalGradeHeader = gradeMethod == Student::GradeMethod::Median ? "Final_Point(Med.)" : "Final_Point(Aver.)";

        std::cout << std::left << std::setw(12) << "Name"
                  << std::left << std::setw(15) << "Surname"
                  << std::right << std::setw(20) << finalGradeHeader << '\n';
        std::cout << std::string(47, '-') << '\n';

        for (const Student& student : students) {
            std::cout << student << '\n';
        }
    }
}

int main() {
    std::size_t inputMode = askChoice("Choose input mode ([1] Manual / [2] Random): ", 1, 2);
    Student::GradeMethod gradeMethod = askGradeMethod();
    std::size_t studentCount = askChoice("Enter number of students: ", 1, 1000);

    std::vector<Student> students;
    students.reserve(studentCount);

    if (inputMode == 1) {
        readManualStudents(students, studentCount, gradeMethod);
    } else {
        readRandomStudents(students, studentCount, gradeMethod);
    }

    printResults(students, gradeMethod);
    return 0;
}