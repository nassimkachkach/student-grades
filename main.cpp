#include "lib.h"
#include "Student.h"

#include <iomanip>
#include <vector>

int main() {
    std::size_t studentCount = 0;
    std::size_t homeworkCount = 0;

    std::cin >> studentCount >> homeworkCount;

    std::vector<Student> students;
    students.reserve(studentCount);

    for (std::size_t i = 0; i < studentCount; ++i) {
        students.emplace_back(homeworkCount);
        std::cin >> students.back();
    }

    std::cout << std::left << std::setw(12) << "Name"
              << std::left << std::setw(15) << "Surname"
              << std::right << std::setw(20) << "Final_Point(Aver.)" << '\n';
    std::cout << std::string(47, '-') << '\n';

    for (const Student& student : students) {
        std::cout << student << '\n';
    }

    return 0;
}