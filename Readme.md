# Student Grades

Student grading and benchmark application with three input modes (manual, random, file), natural sorting, and performance testing of student split strategies across multiple container types.

## Final formula

Final grade = 0.4 x homework value + 0.6 x exam score

where homework value is calculated as:
1. Arithmetic average of homework scores.
2. Median of homework scores.

## Release history

### v0.1
1. Initial student model, grade calculation, and console interaction.
2. Manual and random student input support.

### v0.2
1. File parsing with validation and structured error handling.
2. Sorting by name/surname with natural-order comparison.

### v0.25 (Task 2 baseline)
1. File pipeline timing by phases: reading, sorting, splitting, writing.
2. Container-based processing support (`std::vector`, `std::list`, `std::deque`).

### v1.0 (final)
1. Added split-strategy benchmark menu in file mode.
2. Implemented all three containers for benchmarking.
3. Added two splitting strategies:
   - Strategy 1: copy base students into two new containers (`failed` and `passed`).
   - Strategy 2: move failing students to one new container and keep passed students in the base container.
4. Refactored core logic using `<algorithm>` routines (`std::transform`, `std::copy_if`, `std::remove_copy_if`, `std::copy`, `std::partition`, `std::min_element`, `std::sort`).
5. Added cross-platform build support using `Makefile` (Unix) and `CMakeLists.txt` (any OS).

## Benchmark strategy menu

When choosing file input mode, benchmark menu options are:
1. One container + one strategy.
2. All containers + one strategy.
3. All containers + both strategies.

Split strategies:
1. Strategy 1: original container is unchanged; failed/passed containers are created by copying.
2. Strategy 2: failing students are separated to one container; base container keeps only passed students.

## Measured result snapshot (students_10000.txt)

Benchmark mode: all containers + both strategies, grade method: average, sort by name.

| Container | Strategy | Total us |
| --- | --- | ---: |
| vector | Strategy 1 | 591006 |
| list | Strategy 1 | 640336 |
| deque | Strategy 1 | 765526 |
| vector | Strategy 2 | 581938 |
| list | Strategy 2 | 519198 |
| deque | Strategy 2 | 642403 |

Observed fastest configuration in this run: `std::list` + Strategy 2.

Comment on memory usage:
1. Strategy 1 keeps the original student container and creates two additional containers, so peak memory usage is typically higher.
2. Strategy 2 usually has lower peak memory footprint because only one additional container is created.

## Build guide

### Windows (MSVC build script)

```cmd
build.cmd
```

Produces:
1. `build\main.exe`
2. `build\generate_student_files.exe`

### Unix Makefile

```bash
make
```

Targets:
1. `make` builds `build/main` and `build/generate_student_files`.
2. `make run` runs the main app.
3. `make generate` runs file generator.
4. `make clean` removes `build/`.

### CMake (any OS)

```bash
cmake -S . -B build-cmake
cmake --build build-cmake --config Release
```

Run:
1. `build-cmake/main` on Unix-like systems.
2. `build-cmake/Release/main.exe` on MSVC multi-config generators.

## Application usage

1. Build project (`build.cmd`, `make`, or CMake).
2. Start the main executable.
3. Select input mode:
   - `1` manual
   - `2` random
   - `3` file
4. Select final grade method (`A` or `M`).
5. For file mode, provide input file path (blank defaults to `students10000.txt`).
6. Select sort field (name or surname).
7. Select benchmark mode and strategy/container choices.
8. Inspect generated `*_passed.txt` and `*_failed.txt` files and timing output.

## Input file format

1. Header: `Vardas Pavarde ND1 ... NDn Egz.`
2. One student per line.
3. Whitespace-separated values.
4. Blank lines are ignored.
5. Invalid rows produce explicit parse errors.

## Historical benchmark notes

Earlier v0.25 measurements (hardware-dependent) showed:
1. Parsing and reading dominated total runtime for very large files.
2. `std::list` often sorted fastest due to node relinking in `list::sort()`.
3. `std::vector` was frequently strong in splitting due to contiguous storage.
4. Differences between containers were small on smaller datasets and more visible on million-scale datasets.
