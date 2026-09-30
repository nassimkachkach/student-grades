#include "lib.h"
#include "Student.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
    constexpr std::size_t kMinHomeworkCount = 1;
    constexpr std::size_t kMaxHomeworkCount = 10;

    std::string stripCarriageReturn(std::string line) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        return line;
    }

    std::vector<std::string> splitWhitespace(const std::string& text) {
        std::istringstream stream(text);
        std::vector<std::string> tokens;
        std::string token;

        while (stream >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

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

    bool parseIntToken(const std::string& token, int& value) {
        std::istringstream stream(token);
        if (!(stream >> value)) {
            return false;
        }
        stream >> std::ws;
        return stream.eof();
    }

    std::filesystem::path resolveInputPath(const std::string& inputText) {
        std::filesystem::path candidate(inputText);
        if (candidate.empty()) {
            candidate = "students10000.txt";
        }

        if (candidate.is_absolute()) {
            return candidate;
        }

        if (std::filesystem::exists(candidate)) {
            return candidate;
        }

        const std::filesystem::path workspaceCandidate = std::filesystem::current_path() / candidate;
        if (std::filesystem::exists(workspaceCandidate)) {
            return workspaceCandidate;
        }

        const std::filesystem::path downloadsCandidate = std::filesystem::path("C:/Users/User/Downloads") / candidate;
        if (std::filesystem::exists(downloadsCandidate)) {
            return downloadsCandidate;
        }

        return candidate;
    }

    bool readStudentsFromFile(const std::string& filePath, std::vector<Student>& students) {
        std::ifstream input(filePath);
        if (!input) {
            std::cerr << "Error: unable to open file '" << filePath << "'.\n";
            return false;
        }

        std::string line;
        std::size_t lineNumber = 0;
        std::size_t expectedHomeworkCount = 0;
        bool headerFound = false;

        while (std::getline(input, line)) {
            line = stripCarriageReturn(line);
            ++lineNumber;

            if (line.empty()) {
                continue;
            }

            const std::vector<std::string> columns = splitWhitespace(line);
            if (!headerFound) {
                if (columns.size() < 3) {
                    std::cerr << "Error: malformed header on line " << lineNumber << ": expected at least 3 columns.\n";
                    return false;
                }

                if (columns[0] != "Vardas" || columns[1] != "Pavarde") {
                    std::cerr << "Error: expected a header with 'Vardas Pavarde ... Egz.' on line " << lineNumber << ".\n";
                    return false;
                }

                expectedHomeworkCount = columns.size() - 3;
                if (expectedHomeworkCount == 0) {
                    std::cerr << "Error: no homework columns detected in header on line " << lineNumber << ".\n";
                    return false;
                }

                headerFound = true;
                continue;
            }

            if (columns.size() != expectedHomeworkCount + 3) {
                std::cerr << "Error: malformed row on line " << lineNumber << ": expected "
                          << (expectedHomeworkCount + 3) << " columns, found " << columns.size() << ".\n";
                continue;
            }

            std::vector<int> homeworkScores;
            homeworkScores.reserve(expectedHomeworkCount);
            bool validRow = true;

            for (std::size_t i = 2; i + 1 < columns.size(); ++i) {
                int score = 0;
                if (!parseIntToken(columns[i], score) || score < 1 || score > 10) {
                    std::cerr << "Error: malformed row on line " << lineNumber << ": invalid homework score '"
                              << columns[i] << "'.\n";
                    validRow = false;
                    break;
                }
                homeworkScores.push_back(score);
            }

            if (!validRow) {
                continue;
            }

            int examScore = 0;
            if (!parseIntToken(columns.back(), examScore) || examScore < 0 || examScore > 10) {
                std::cerr << "Error: malformed row on line " << lineNumber << ": invalid exam score '"
                          << columns.back() << "'.\n";
                continue;
            }

            Student student(columns[0], columns[1], homeworkScores, examScore);
            student.calculateFinalGrades();
            students.push_back(student);
        }

        if (!headerFound) {
            std::cerr << "Error: file '" << filePath << "' does not contain a valid header.\n";
            return false;
        }

        return true;
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
    std::size_t inputMode = askChoice("Choose input mode ([1] Manual / [2] Random / [3] File): ", 1, 3);
    Student::GradeMethod gradeMethod = askGradeMethod();
    std::vector<Student> students;

    if (inputMode == 1) {
        std::size_t studentCount = askChoice("Enter number of students: ", 1, 1000);
        students.reserve(studentCount);
        readManualStudents(students, studentCount, gradeMethod);
    } else if (inputMode == 2) {
        std::size_t studentCount = askChoice("Enter number of students: ", 1, 1000);
        students.reserve(studentCount);
        readRandomStudents(students, studentCount, gradeMethod);
    } else {
        std::string filePath = askText("Enter file path (blank uses students10000.txt): ");
        const std::filesystem::path resolvedPath = resolveInputPath(filePath);
        if (!readStudentsFromFile(resolvedPath.string(), students)) {
            return 1;
        }
    }

    std::size_t sortChoice = askChoice("Sort by ([1] Name / [2] Surname): ", 1, 2);
    if (sortChoice == 1) {
        std::sort(students.begin(), students.end(), compareStudentsByName);
    } else {
        std::sort(students.begin(), students.end(), compareStudentsBySurname);
    }

    printResults(students);
    return 0;
}