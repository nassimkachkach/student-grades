cmake_minimum_required(VERSION 3.16)
project(student_grades LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_executable(main
    main.cpp
    Student.cpp
    StudentFileReader.cpp
    StudentFileWriter.cpp
    RandomStudentGenerator.cpp
    Sorting.cpp
    StudentFileGenerator.cpp
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(generate_student_files
    GenerateStudentFiles.cpp
    StudentFileGenerator.cpp
    Student.cpp
    RandomStudentGenerator.cpp
)

target_include_directories(generate_student_files PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
