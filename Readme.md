# Student Grades

This program reads student data from manual input, random generation, or a file, calculates the final grade, and prints results sorted by name or surname.

## Final formula

Final grade = 0.4 × homework value + 0.6 × exam score

where the homework value is either:
- the arithmetic average of homework scores, or
- the median of homework scores

depending on the selected grade method.

## Build

From the project root:

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x64 -host_arch=x64
cl /std:c++17 /EHsc /W4 /nologo /I. /Fo:build\ /Fe:build\main.exe main.cpp Student.cpp
```

## Usage

Run the compiled program:

```cmd
build\main.exe
```

At runtime you can choose:
1. Manual input
2. Random generation
3. File input

For file input, the expected format is:
- header: `Vardas Pavarde ND1 ... ND15 Egz.`
- one student per line
- whitespace-separated values
- CRLF line endings are accepted
- blank lines are ignored
- rows with invalid data are skipped with a clear error message

The program also supports sorting by either name or surname using `std::sort` and a natural-order comparator, so `Vardas10` sorts after `Vardas9`.