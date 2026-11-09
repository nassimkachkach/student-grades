#include "lib.h"
#include "Student.h"
#include "RandomStudentGenerator.h"
#include "Sorting.h"
#include "StudentFileReader.h"
#include "StudentFileWriter.h"

#include <algorithm>
#include <chrono>
#include <deque>
#include <exception>
#include <iomanip>
#include <filesystem>
#include <list>
#include <limits>
#include <string>
#include <type_traits>
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
        Microseconds total{};
    };

    enum class ContainerChoice {
        Vector = 1,
        List = 2,
        Deque = 3
    };

    enum class SplitStrategy {
        Strategy1 = 1,
        Strategy2 = 2
    };

    template <typename Container>
    struct SplitStudents {
        Container passed;
        Container failed;
    };

    struct PipelineResult {
        std::string containerName;
        SplitStrategy strategy{SplitStrategy::Strategy1};
        std::size_t passedCount{0};
        std::size_t failedCount{0};
        std::filesystem::path passedPath;
        std::filesystem::path failedPath;
        PhaseTimings timings;
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

    template <typename Container>
    std::size_t homeworkCountForStudents(const Container& students) {
        if (students.empty()) {
            throw std::runtime_error("No students were loaded from the input file.");
        }

        return students.front().homeworkResults().size();
    }

    std::string splitStrategyName(SplitStrategy strategy) {
        if (strategy == SplitStrategy::Strategy1) {
            return "Strategy 1";
        }
        return "Strategy 2";
    }

    std::size_t askBenchmarkModeChoice() {
        return askChoice(
            "Benchmark mode ([1] One container + one strategy / [2] All containers + one strategy / [3] All containers + both strategies): ",
            1,
            3);
    }

    ContainerChoice askContainerChoice() {
        const std::size_t choice = askChoice("Container ([1] Vector / [2] List / [3] Deque): ", 1, 3);
        return static_cast<ContainerChoice>(choice);
    }

    SplitStrategy askSplitStrategyChoice() {
        const std::size_t choice = askChoice(
            "Split strategy ([1] Strategy 1: copy into two new containers / [2] Strategy 2: move failed, shrink base container): ",
            1,
            2);
        return static_cast<SplitStrategy>(choice);
    }

    template <typename Container>
    std::string containerNameForType() {
        if constexpr (std::is_same_v<Container, std::list<Student>>) {
            return "list";
        } else if constexpr (std::is_same_v<Container, std::deque<Student>>) {
            return "deque";
        } else {
            return "vector";
        }
    }

    template <typename Container>
    void calculateFinalGrades(Container& students, Student::GradeMethod gradeMethod) {
        std::transform(
            students.begin(),
            students.end(),
            students.begin(),
            [gradeMethod](Student student) {
                student.calculateFinalGrade(gradeMethod);
                return student;
            });
    }

    template <typename Container>
    void sortStudents(Container& students, std::size_t sortChoice) {
        const auto comparator = (sortChoice == 1) ? StudentComparator::compareByName : StudentComparator::compareBySurname;

        if constexpr (std::is_same_v<Container, std::list<Student>>) {
            students.sort(comparator);
        } else {
            std::sort(students.begin(), students.end(), comparator);
        }
    }

    template <typename Container>
    SplitStudents<Container> splitStudentsStrategy1(const Container& students) {
        SplitStudents<Container> result;

        std::copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(result.failed),
            [](const Student& student) {
                return student.finalGrade() < 5.0;
            });

        std::remove_copy_if(
            students.begin(),
            students.end(),
            std::back_inserter(result.passed),
            [](const Student& student) {
                return student.finalGrade() < 5.0;
            });

        return result;
    }

    template <typename Container>
    SplitStudents<Container> splitStudentsStrategy2(Container& students) {
        SplitStudents<Container> result;

        if constexpr (std::is_same_v<Container, std::list<Student>>) {
            for (auto it = students.begin(); it != students.end();) {
                if (it->finalGrade() < 5.0) {
                    auto current = it++;
                    result.failed.splice(result.failed.end(), students, current);
                } else {
                    ++it;
                }
            }
        } else {
            const auto failedBoundary = std::partition(
                students.begin(),
                students.end(),
                [](const Student& student) {
                    return student.finalGrade() < 5.0;
                });

            std::move(students.begin(), failedBoundary, std::back_inserter(result.failed));
            students.erase(students.begin(), failedBoundary);
        }

        std::copy(students.begin(), students.end(), std::back_inserter(result.passed));
        return result;
    }

    template <typename Container>
    SplitStudents<Container> splitStudents(Container& students, SplitStrategy strategy) {
        if (strategy == SplitStrategy::Strategy1) {
            return splitStudentsStrategy1(students);
        }

        return splitStudentsStrategy2(students);
    }

    void printPhaseTimings(const PhaseTimings& timings) {
        std::cout << "Phase timings (microseconds):\n"
                  << "  Reading:          " << timings.reading.count() << '\n'
                  << "  Sorting:          " << timings.sorting.count() << '\n'
                  << "  Splitting:        " << timings.splitting.count() << '\n'
                  << "  Writing passed:    " << timings.writingPassed.count() << '\n'
                  << "  Writing failed:    " << timings.writingFailed.count() << '\n'
                  << "  Total:            " << timings.total.count() << '\n';
    }

    template <typename Container>
    PipelineResult processFilePipeline(const std::filesystem::path& resolvedPath,
                                       Student::GradeMethod gradeMethod,
                                       std::size_t sortChoice,
                                       SplitStrategy strategy) {
        PipelineResult result;
        result.containerName = containerNameForType<Container>();
        result.strategy = strategy;

        const auto totalStart = Clock::now();
        auto phaseStart = Clock::now();

        std::vector<Student> loadedStudents;
        StudentFileReader::readStudentsFromFile(resolvedPath.string(), loadedStudents);
        Container students(loadedStudents.begin(), loadedStudents.end());
        result.timings.reading = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        const std::size_t homeworkCount = homeworkCountForStudents(students);

        phaseStart = Clock::now();
        sortStudents(students, sortChoice);
        result.timings.sorting = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        phaseStart = Clock::now();
        calculateFinalGrades(students, gradeMethod);
        const SplitStudents<Container> splitStudentsResult = splitStudents(students, strategy);
        result.timings.splitting = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        const std::string suffixBase = "_" + result.containerName + "_s" + std::to_string(static_cast<int>(strategy));
        result.passedPath = makeSiblingOutputPath(resolvedPath, suffixBase + "_passed");
        result.failedPath = makeSiblingOutputPath(resolvedPath, suffixBase + "_failed");

        phaseStart = Clock::now();
        StudentFileWriter::writeStudentsToFile(result.passedPath, splitStudentsResult.passed, homeworkCount);
        result.timings.writingPassed = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        phaseStart = Clock::now();
        StudentFileWriter::writeStudentsToFile(result.failedPath, splitStudentsResult.failed, homeworkCount);
        result.timings.writingFailed = std::chrono::duration_cast<Microseconds>(Clock::now() - phaseStart);

        result.timings.total = std::chrono::duration_cast<Microseconds>(Clock::now() - totalStart);
        result.passedCount = splitStudentsResult.passed.size();
        result.failedCount = splitStudentsResult.failed.size();

        return result;
    }

    void printPipelineResult(const PipelineResult& result) {
        std::cout << "Container: " << result.containerName
                  << ", " << splitStrategyName(result.strategy) << '\n';
        std::cout << "Passed file: " << result.passedPath.string() << " (" << result.passedCount << " students)\n";
        std::cout << "Failed file: " << result.failedPath.string() << " (" << result.failedCount << " students)\n";
        printPhaseTimings(result.timings);
    }

    void printBenchmarkSummary(const std::vector<PipelineResult>& results) {
        if (results.empty()) {
            return;
        }

        std::cout << "\nBenchmark summary (microseconds):\n";
        std::cout << std::left << std::setw(10) << "Container"
                  << std::left << std::setw(12) << "Strategy"
                  << std::right << std::setw(14) << "Reading"
                  << std::right << std::setw(14) << "Sorting"
                  << std::right << std::setw(14) << "Splitting"
                  << std::right << std::setw(14) << "Write pass"
                  << std::right << std::setw(14) << "Write fail"
                  << std::right << std::setw(14) << "Total" << '\n';

        for (const PipelineResult& result : results) {
            std::cout << std::left << std::setw(10) << result.containerName
                      << std::left << std::setw(12) << splitStrategyName(result.strategy)
                      << std::right << std::setw(14) << result.timings.reading.count()
                      << std::right << std::setw(14) << result.timings.sorting.count()
                      << std::right << std::setw(14) << result.timings.splitting.count()
                      << std::right << std::setw(14) << result.timings.writingPassed.count()
                      << std::right << std::setw(14) << result.timings.writingFailed.count()
                      << std::right << std::setw(14) << result.timings.total.count() << '\n';
        }

        const auto best = std::min_element(
            results.begin(),
            results.end(),
            [](const PipelineResult& left, const PipelineResult& right) {
                return left.timings.total < right.timings.total;
            });

        std::cout << "\nFastest configuration: " << best->containerName
                  << " with " << splitStrategyName(best->strategy)
                  << " (total " << best->timings.total.count() << " us).\n";
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
            const std::size_t benchmarkMode = askBenchmarkModeChoice();

            std::vector<PipelineResult> results;

            const auto runSingleConfiguration = [&](ContainerChoice choice, SplitStrategy strategy) {
                if (choice == ContainerChoice::List) {
                    results.push_back(processFilePipeline<std::list<Student>>(resolvedPath, gradeMethod, sortChoice, strategy));
                } else if (choice == ContainerChoice::Deque) {
                    results.push_back(processFilePipeline<std::deque<Student>>(resolvedPath, gradeMethod, sortChoice, strategy));
                } else {
                    results.push_back(processFilePipeline<std::vector<Student>>(resolvedPath, gradeMethod, sortChoice, strategy));
                }
            };

            if (benchmarkMode == 1) {
                runSingleConfiguration(askContainerChoice(), askSplitStrategyChoice());
            } else if (benchmarkMode == 2) {
                const SplitStrategy strategy = askSplitStrategyChoice();
                runSingleConfiguration(ContainerChoice::Vector, strategy);
                runSingleConfiguration(ContainerChoice::List, strategy);
                runSingleConfiguration(ContainerChoice::Deque, strategy);
            } else {
                for (SplitStrategy strategy : {SplitStrategy::Strategy1, SplitStrategy::Strategy2}) {
                    runSingleConfiguration(ContainerChoice::Vector, strategy);
                    runSingleConfiguration(ContainerChoice::List, strategy);
                    runSingleConfiguration(ContainerChoice::Deque, strategy);
                }
            }

            for (const PipelineResult& result : results) {
                printPipelineResult(result);
                std::cout << '\n';
            }

            if (results.size() > 1) {
                printBenchmarkSummary(results);
            }

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