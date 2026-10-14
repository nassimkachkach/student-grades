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
build.cmd
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
- rows with invalid data stop parsing and report a clear error message

The program also supports sorting by either name or surname using `std::sort` and a natural-order comparator, so `Vardas10` sorts after `Vardas9`.

## Test files, splitting and speed analysis

Generate the test files (1 000, 10 000, 100 000, 1 000 000 and 10 000 000 students named `Name1 Surname1`, `Name2 Surname2`, ... with 10 random homework scores and an exam score):

```cmd
build\generate_student_files.exe
```

They are written to `generated\students_<N>.txt`. Choosing file input in `build\main.exe` then:

1. reads the file,
2. sorts the students,
3. splits them into **passed** (final >= 5.0) and **failed** (final < 5.0),
4. writes them to `<file>_passed.txt` and `<file>_failed.txt`,

and prints the time of every step in microseconds.

Since v0.25 the program asks which container to use: `std::vector`, `std::list` or `std::deque`. `StudentFileReader` and `StudentFileWriter` are templates instantiated for all three, and `std::list` is sorted with its own `list::sort()` because `std::sort` needs random access iterators.

`build.cmd` builds a 64-bit program with `/O2`. A 32-bit build runs out of memory (`bad allocation`) on 10 000 000 records.

### Results

Intel Core i5-10300H @ 2.50 GHz, 16 GB RAM, SSD, Windows 11, MSVC 64-bit `/O2`. Sorted by surname, final grade from the homework average. Times in seconds; Write is both files together.

**v0.2 - `std::vector`**

| Records | Read | Sort | Split | Write | Total |
|---:|---:|---:|---:|---:|---:|
| 1 000 | 0.0097 | 0.0006 | 0.0002 | 0.0418 | 0.0522 |
| 10 000 | 0.1053 | 0.0100 | 0.0023 | 0.0432 | 0.1608 |
| 100 000 | 1.0291 | 0.1514 | 0.0314 | 0.4052 | 1.6171 |
| 1 000 000 | 10.8195 | 2.3307 | 0.3780 | 5.1930 | 18.7211 |
| 10 000 000 | 98.8147 | 21.0721 | 2.5225 | 34.9222 | 157.3314 |

**v0.25 - `std::vector`**

| Records | Read | Sort | Split | Write | Total |
|---:|---:|---:|---:|---:|---:|
| 1 000 | 0.0096 | 0.0006 | 0.0003 | 0.0059 | 0.0165 |
| 10 000 | 0.0867 | 0.0097 | 0.0021 | 0.0380 | 0.1365 |
| 100 000 | 0.8903 | 0.1315 | 0.0217 | 0.3510 | 1.3945 |
| 1 000 000 | 9.2478 | 1.7029 | 0.2308 | 5.8835 | 17.0651 |
| 10 000 000 | 99.7898 | 21.7335 | 3.4604 | 37.4903 | 162.4740 |

**v0.25 - `std::list`**

| Records | Read | Sort | Split | Write | Total |
|---:|---:|---:|---:|---:|---:|
| 1 000 | 0.0112 | 0.0004 | 0.0004 | 0.0089 | 0.0208 |
| 10 000 | 0.0915 | 0.0053 | 0.0031 | 0.0365 | 0.1363 |
| 100 000 | 0.8578 | 0.0787 | 0.0311 | 0.3503 | 1.3179 |
| 1 000 000 | 8.5576 | 0.9554 | 0.3186 | 3.6115 | 13.4430 |
| 10 000 000 | 89.8780 | 14.8394 | 5.1092 | 39.2666 | 149.0932 |

**v0.25 - `std::deque`**

| Records | Read | Sort | Split | Write | Total |
|---:|---:|---:|---:|---:|---:|
| 1 000 | 0.0125 | 0.0011 | 0.0007 | 0.0104 | 0.0246 |
| 10 000 | 0.1186 | 0.0103 | 0.0034 | 0.0393 | 0.1717 |
| 100 000 | 0.9354 | 0.1667 | 0.0324 | 0.3517 | 1.4862 |
| 1 000 000 | 8.8977 | 1.8129 | 0.3237 | 3.6242 | 14.6585 |
| 10 000 000 | 91.6319 | 23.9105 | 3.6663 | 44.5860 | 163.7948 |

### Conclusions

- Reading takes about 60% of the total time with every container. It is dominated by parsing the text (`std::istringstream` per line), not by inserting into the container.
- `std::list` sorts fastest: `list::sort()` relinks nodes, while `std::sort` on a `vector` or `deque` has to copy whole `Student` objects (the class has a user-written copy constructor, so there is no move).
- Splitting is fastest with `std::vector`, because `reserve()` avoids reallocations; `list` and `deque` allocate memory for every copied student.
- All three containers produce identical passed and failed files.
- Up to 100 000 records the differences are only a few milliseconds.
- With 10 000 000 records the program needs several GB of memory, so the timings also depend on how much free RAM the computer has (the runs above had less than 1 GB free and used the page file).