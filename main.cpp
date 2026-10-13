#include "lib.h"
#include "Student.h"
#include "RandomStudentGenerator.h"
#include "Sorting.h"
#include "StudentFileReader.h"
#include "StudentFileWriter.h"

#include <algorithm>
#include <chrono>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

namespace {
    using Clock = std::chrono::steady_clock;
    using Microseconds = std::chrono::microseconds;

    struct PhaseTimings {
        Microseconds reading{};
        Microseconds sorting{};
        Microseconds splitting{};
        Microseconds writingPassed{};
        Microseconds writingFailed{};
    };

    struct SplitStudents {
        std::vector<Student> passed;
        std::vector<Student> failed;
    };

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

    std::filesystem::path makeSiblingOutputPath(const std::filesystem::path& inputPath, const std::string& suffix) {
        const std::filesystem::path parentPath = inputPath.parent_path();
        const std::string baseName = inputPath.stem().string() + suffix + inputPath.extension().string();
        return parentPath.empty() ? std::filesystem::path(baseName) : parentPath / baseName;
    }

    std::size_t homeworkCountForStudents(const std::vector<Student>& students) {
        if (students.empty()) {
            throw std::runtime_error("No students were loaded from the input file.");
        }

        return students.front().homeworkResults().size();
    }

    void sortStudents(std::vector<Student>& students, std::size_t sortChoice) {
        if (sortChoice == 1) {
            std::sort(students.begin(), students.end(), StudentComparator::compareByName);
        } else {
            std::sort(students.begin(), students.end(), StudentComparator::compareBySurname);
        }
    }

    SplitStudents splitStudents(std::vector<Student>& students, Student::GradeMethod gradeMethod) {
        SplitStudents result;
        result.passed.reserve(students.size());
        result.failed.reserve(students.size());

        for (Student& student : students) {
            student.calculateFinalGrade(gradeMethod);

            if (student.finalGrade() < 5.0) {
                result.failed.push_back(student);
            } else {
                result.passed.push_back(student);
            }
        }

        return result;
    }

    void printPhaseTimings(const PhaseTimings& timings) {
        std::cout << "Phase timings (microseconds):\n"
                  << "  Reading:          " << timings.reading.count() << '\n'
                  << "  Sorting:          " << timings.sorting.count() << '\n'
                  << "  Splitting:        " << timings.splitting.count() << '\n'
                  << "  Writing passed:    " << timings.writingPassed.count() << '\n'
                  << "  Writing failed:    " << timings.writingFailed.count() << '\n';
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

    void processFilePipeline(const std::filesystem::path& resolvedPath, Student::GradeMethod gradeMethod, std::size_t sortChoice) {
        std::vector<Student> students;
        PhaseTimings timings;

        auto phaseStart = Clock::now();
        StudentFileReader::readStudentsFromFile(resolvedPath.string(), students);
        timings.reading = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        const std::size_t homeworkCount = homeworkCountForStudents(students);

        phaseStart = Clock::now();
        sortStudents(students, sortChoice);
        timings.sorting = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        phaseStart = Clock::now();
        const SplitStudents splitStudentsResult = splitStudents(students, gradeMethod);
        timings.splitting = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        const std::filesystem::path passedPath = makeSiblingOutputPath(resolvedPath, "_passed");
        const std::filesystem::path failedPath = makeSiblingOutputPath(resolvedPath, "_failed");

        phaseStart = Clock::now();
        StudentFileWriter::writeStudentsToFile(passedPath, splitStudentsResult.passed, homeworkCount);
        timings.writingPassed = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        phaseStart = Clock::now();
        StudentFileWriter::writeStudentsToFile(failedPath, splitStudentsResult.failed, homeworkCount);
        timings.writingFailed = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        std::cout << "Passed file: " << passedPath.string() << " (" << splitStudentsResult.passed.size() << " students)\n";
        std::cout << "Failed file: " << failedPath.string() << " (" << splitStudentsResult.failed.size() << " students)\n";
        printPhaseTimings(timings);
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
                    throw std::runtime_error("Invalid student input.");
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
            std::size_t sortChoice = askChoice("Sort by ([1] Name / [2] Surname): ", 1, 2);
            processFilePipeline(resolvedPath, gradeMethod, sortChoice);
            return 0;
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