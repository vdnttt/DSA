# Smart Student Search & Ranking System

A console-based Data Structures project in C that stores student records in an
array of structures and demonstrates searching, sorting and ranking algorithms,
with real comparison and swap counts for each algorithm.

## Problem Statement
Student records must be stored, found quickly, ordered and ranked by marks.
Different searching and sorting algorithms behave very differently as data grows.
This project implements the algorithms from scratch and measures them on real
data so their efficiency can be compared and understood.

## Features
- Add students with input validation (unique positive ID, semester 1-8, marks 0-100)
- Display records in a formatted table
- Search by ID with 4 algorithms
- Sort by marks (descending), ID or name with 8 algorithms
- Ranking by marks with ties sharing a rank (95, 92, 92, 85, 78 -> 1, 2, 2, 4, 5)
- Top 3 / Top 5 / Top 10 students
- Algorithm comparison using actual counted comparisons, swaps and measured time
- Built-in theoretical complexity table

## Data Structures Used
- `struct Student` (id, name, branch, semester, marks, rank)
- A fixed-size array of structures (`MAX_STUDENTS = 100`) plus a record counter
- Fixed-size auxiliary arrays (merge sort temp array, radix output array,
  2D array of buckets for bucket sort)

No linked lists, trees, graphs, heaps or hash tables are used.

## Algorithms Used
**Searching:** Linear, Sentinel, Binary, Fibonacci
(Binary and Fibonacci search a sorted temporary copy; the original data is not changed.)

**Sorting:** Bubble, Selection, Insertion, Shell, Merge, Quick, Radix, Bucket
(Radix and Bucket sort work on numeric keys, so they support marks and ID only.)

**Ranking:** merge sort by marks, then one pass that assigns ranks (ties share a rank).

## Complexity Analysis
| Algorithm | Best | Average | Worst | Space |
|---|---|---|---|---|
| Linear Search | O(1) | O(n) | O(n) | O(1) |
| Sentinel Search | O(1) | O(n) | O(n) | O(1) |
| Binary Search | O(1) | O(log n) | O(log n) | O(1) |
| Fibonacci Search | O(1) | O(log n) | O(log n) | O(1) |
| Bubble Sort | O(n) | O(n^2) | O(n^2) | O(1) |
| Selection Sort | O(n^2) | O(n^2) | O(n^2) | O(1) |
| Insertion Sort | O(n) | O(n^2) | O(n^2) | O(1) |
| Shell Sort | O(n log n) | depends on gaps | O(n^2) | O(1) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) |
| Quick Sort | O(n log n) | O(n log n) | O(n^2) | O(log n) |
| Radix Sort | O(d(n+k)) | O(d(n+k)) | O(d(n+k)) | O(n+k) |
| Bucket Sort | O(n+k) | O(n+k) | O(n^2) | O(n+k) |

## Technologies Used
- C (standard library only: stdio, stdlib, string, ctype, time)
- GCC (MinGW-w64)
- Git and GitHub

## Development Environment
Windows, Visual Studio Code with the Microsoft C/C++ extension, GCC 15.2.0 (MinGW-w64), GDB.

## How to Compile
```
gcc -Wall -Wextra -g -Iinclude src/main.c src/student.c src/searching.c src/sorting.c src/analysis.c -o ranking.exe
```
A single-file version is in `single/main.c`:
```
gcc -Wall -Wextra -g single/main.c -o ranking_single.exe
```

## How to Run
```
.\ranking.exe
```

## Example Output
Ranking with ties:
```
Rank   ID       Name              Branch       Marks
---------------------------------------------------------
1      1        g                 f            95.0
2      2        f                 g            92.0
2      3        d                 e            92.0
4      4        h                 i            87.0
5      5        f                 g            78.0
---------------------------------------------------------
```
Sorting comparison (100 random records, by marks; a Time column is also printed and varies by machine):
```
Algorithm         Comparisons    Swaps/Moves
---------------------------------------------
Bubble Sort              4884           2591
Selection Sort           4950             98
Insertion Sort           2685           2683
Shell Sort                870            648
Merge Sort                546            672
Quick Sort                574            261
Radix Sort                  0            400
Bucket Sort               348            443
```
Search comparison (5 records, key 7 not present):
```
Linear Search                 5   Not found
Sentinel Search               5   Not found
Binary Search                 3   Not found
Fibonacci Search              4   Not found
```

## Project Structure
```
Smart-Student-Search-Ranking/
├── src/       main.c, student.c, searching.c, sorting.c, analysis.c
├── include/   student.h, searching.h, sorting.h, analysis.h
├── single/    main.c  (all modules merged into one file)
├── README.md
├── .gitignore
└── LICENSE
```

## Notes
- Sorting and ranking reorder the stored array in place.
- Merge sort is stable, so tied students keep their entry order.
- Quick sort uses the last element as pivot, so its worst case occurs on already-sorted input.

## Future Improvements
- Save and load records from a file
- Update and delete records
- Search by name
- Random pivot or median-of-three for quick sort
- Additional gap sequences for Shell sort
- Larger or dynamically sized storage (after studying linked lists)

## Author
Manav Nalkande, CSE CSF, MIT-WPU
Vedant Puri, CSE CSF, MIT-WPU
Bhumil Kiyada, CSE CSF, MIT-WPU
Soham Nalawade, CSE CSF, MIT-WPU
Sanskar Thitame, CSE CSF, MIT-WPU