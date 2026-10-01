#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "analysis.h"
#include "searching.h"
#include "sorting.h"



static void copyArray(const struct Student src[], struct Student dest[], int n) {
    int i;
    for (i = 0; i < n; i++) dest[i] = src[i];
}

/* Run sorting algorithm number 0..7 by MARKS. */
static void runSort(int algo, struct Student a[], int n) {
    switch (algo) {
        case 0: bubbleSort(a, n, SORT_BY_MARKS); break;
        case 1: selectionSort(a, n, SORT_BY_MARKS); break;
        case 2: insertionSort(a, n, SORT_BY_MARKS); break;
        case 3: shellSort(a, n, SORT_BY_MARKS); break;
        case 4: mergeSort(a, n, SORT_BY_MARKS); break;
        case 5: quickSort(a, n, SORT_BY_MARKS); break;
        case 6: radixSort(a, n, SORT_BY_MARKS); break;
        default: bucketSort(a, n, SORT_BY_MARKS); break;
    }
}

/* SORTING COMPARISON: every algorithm gets a fresh copy of the same data.
   Comparisons/swaps come from ONE real run (counters in sorting.c).
   Time = average over TIMING_REPEATS runs (one run is too short for clock()). */
void compareSortingAlgorithms(const struct Student students[], int count) {
    static const char *names[8] = {"Bubble Sort", "Selection Sort", "Insertion Sort",
        "Shell Sort", "Merge Sort", "Quick Sort", "Radix Sort", "Bucket Sort"};
    static struct Student base[MAX_STUDENTS], work[MAX_STUDENTS];
    long comps[8], moves[8];
    double micros[8];
    int n, choice, algo, r, i;
    long runs;
    clock_t start, end;

    printf("\n1. Use my current records (%d)\n2. Generate random test data\n", count);
    choice = readInt("Choose data: ", 1, 2);

    if (choice == 1) {
        if (count == 0) {
            printf("No records. Add students first.\n");
            return;
        }
        n = count;
        copyArray(students, base, n);
    } else {
        n = readInt("How many random records (10-100): ", 10, MAX_STUDENTS);
        srand((unsigned)time(NULL));
        for (i = 0; i < n; i++) {
            base[i].id = i + 1;
            strcpy(base[i].name, "Test");
            strcpy(base[i].branch, "Test");
            base[i].semester = 1;
            base[i].marks = (rand() % 1001) / 10.0f;   /* 0.0 to 100.0 */
            base[i].rank = 0;
        }
    }

    for (algo = 0; algo < 8; algo++) {
        copyArray(base, work, n);
        sortComparisons = 0;
        sortSwaps = 0;
        runSort(algo, work, n);
        comps[algo] = sortComparisons;
        moves[algo] = sortSwaps;

        runs = 0;
        start = clock();
        do {                                   /* repeat until at least 0.2 s have passed */
            for (r = 0; r < 100; r++) {
                copyArray(base, work, n);
                runSort(algo, work, n);
            }
            runs += 100;
            end = clock();
        } while ((end - start) < CLOCKS_PER_SEC / 5);
        micros[algo] = (double)(end - start) * 1000000.0 / CLOCKS_PER_SEC / runs;
    }

    printf("\n================ SORTING COMPARISON (n = %d, by marks) ================\n\n", n);
    printf("%-16s %12s %14s %14s\n", "Algorithm", "Comparisons", "Swaps/Moves", "Time (us)");
    printf("------------------------------------------------------------\n");
    for (algo = 0; algo < 8; algo++) {
        printf("%-16s %12ld %14ld %14.2f\n", names[algo], comps[algo], moves[algo], micros[algo]);
    }
    printf("------------------------------------------------------------\n");
    printf("Radix sort never compares elements. Bucket sort counts only the\n");
    printf("comparisons inside its buckets. Merge/Radix/Bucket 'moves' are copies.\n");
    printf("Time includes copying the data (same overhead for every algorithm).\n");
}

/* SEARCH COMPARISON: same key, real comparison counts.
   Linear and Sentinel use the original order; Binary and Fibonacci use a
   sorted-by-ID copy (they require sorted data). */
void compareSearchingAlgorithms(const struct Student students[], int count) {
    struct Student original[MAX_STUDENTS], sorted[MAX_STUDENTS];
    static const char *names[4] = {"Linear Search", "Sentinel Search",
                                   "Binary Search", "Fibonacci Search"};
    long comps[4];
    int result[4], key, i;

    if (count == 0) {
        printf("No records. Add students first.\n");
        return;
    }

    key = readInt("Enter Student ID to search for: ", 1, 999999);

    copyArray(students, original, count);        /* sentinel search edits temporarily */
    sortCopyById(students, sorted, count);

    searchComparisons = 0; result[0] = linearSearch(original, count, key);    comps[0] = searchComparisons;
    searchComparisons = 0; result[1] = sentinelSearch(original, count, key);  comps[1] = searchComparisons;
    searchComparisons = 0; result[2] = binarySearch(sorted, count, key);      comps[2] = searchComparisons;
    searchComparisons = 0; result[3] = fibonacciSearch(sorted, count, key);   comps[3] = searchComparisons;

    printf("\n================ SEARCH COMPARISON ================\n\n");
    printf("Search Key: %d   (records: %d)\n\n", key, count);
    printf("%-18s %12s   %s\n", "Algorithm", "Comparisons", "Result");
    printf("-------------------------------------------------\n");
    for (i = 0; i < 4; i++) {
        printf("%-18s %12ld   %s\n", names[i], comps[i], result[i] >= 0 ? "Found" : "Not found");
    }
    printf("-------------------------------------------------\n");
    printf("Binary and Fibonacci searched a sorted copy of the data.\n");
}

/* Theoretical complexity (n = number of records, d = digits in the largest key,
   k = range of digit values / number of buckets). Plain ASCII so the Windows
   console displays it correctly. */
void displayComplexityTable(void) {
    printf("\n============================ COMPLEXITY TABLE ============================\n\n");
    printf("%-18s %-12s %-12s %-12s %s\n", "Algorithm", "Best", "Average", "Worst", "Space");
    printf("--------------------------------------------------------------------------\n");
    printf("%-18s %-12s %-12s %-12s %s\n", "Linear Search",    "O(1)", "O(n)", "O(n)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Sentinel Search",  "O(1)", "O(n)", "O(n)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Binary Search",    "O(1)", "O(log n)", "O(log n)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Fibonacci Search", "O(1)", "O(log n)", "O(log n)", "O(1)");
    printf("--------------------------------------------------------------------------\n");
    printf("%-18s %-12s %-12s %-12s %s\n", "Bubble Sort",    "O(n)", "O(n^2)", "O(n^2)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Selection Sort", "O(n^2)", "O(n^2)", "O(n^2)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Insertion Sort", "O(n)", "O(n^2)", "O(n^2)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Shell Sort",     "O(n log n)", "gap-based", "O(n^2)", "O(1)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Merge Sort",     "O(n log n)", "O(n log n)", "O(n log n)", "O(n)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Quick Sort",     "O(n log n)", "O(n log n)", "O(n^2)", "O(log n)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Radix Sort",     "O(d(n+k))", "O(d(n+k))", "O(d(n+k))", "O(n+k)");
    printf("%-18s %-12s %-12s %-12s %s\n", "Bucket Sort",    "O(n+k)", "O(n+k)", "O(n^2)", "O(n+k)");
    printf("--------------------------------------------------------------------------\n");
    printf("Shell sort's average depends on the gap sequence (ours: n/2, n/4, ... 1).\n");
    printf("Quick sort's space is the recursion stack. Merge/Radix/Bucket need extra arrays.\n");
}

void algorithmComparisonMenu(const struct Student students[], int count) {
    int choice;
    printf("\n1. Sorting Comparison\n2. Search Comparison\n3. Complexity Table\n");
    choice = readInt("Choose: ", 1, 3);
    if (choice == 1)      compareSortingAlgorithms(students, count);
    else if (choice == 2) compareSearchingAlgorithms(students, count);
    else                  displayComplexityTable();
}