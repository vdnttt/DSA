#include <stdio.h>
#include "searching.h"

long searchComparisons = 0;

/* LINEAR SEARCH: check every element from the start until found.
   Best O(1), Average/Worst O(n). Works on unsorted data. */
int linearSearch(const struct Student a[], int n, int key) {
    int i;
    for (i = 0; i < n; i++) {
        searchComparisons++;
        if (a[i].id == key) return i;
    }
    return -1;
}

/* SENTINEL SEARCH: put the key at the last position as a "sentinel" so the
   loop needs no "i < n" check. The last element is saved and restored, so
   the data is unchanged afterwards. Still O(n), but with fewer checks per step.
   Only key comparisons are counted (the removed bounds check is the saving). */
int sentinelSearch(struct Student a[], int n, int key) {
    struct Student last;
    int i = 0;

    if (n == 0) return -1;

    last = a[n - 1];
    a[n - 1].id = key;                 /* place sentinel */

    while (1) {
        searchComparisons++;
        if (a[i].id == key) break;     /* guaranteed to stop at the sentinel */
        i++;
    }

    a[n - 1] = last;                   /* restore original element */

    if (i < n - 1 || last.id == key) return i;
    return -1;                         /* we only found the sentinel */
}

/* BINARY SEARCH: repeatedly compare with the middle element and discard half.
   REQUIRES data sorted by ID. Best O(1), Average/Worst O(log n).
   One comparison is counted per middle element examined. */
int binarySearch(const struct Student a[], int n, int key) {
    int low = 0, high = n - 1, mid;

    while (low <= high) {
        mid = low + (high - low) / 2;
        searchComparisons++;
        if (a[mid].id == key) return mid;
        else if (a[mid].id < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* FIBONACCI SEARCH: like binary search, but splits the range using Fibonacci
   numbers (only addition/subtraction, no division). REQUIRES sorted data.
   fib = F(k) is the smallest Fibonacci number >= n; fib1 = F(k-1); fib2 = F(k-2).
   offset marks the part already eliminated from the front.
   Best O(1), Average/Worst O(log n). */
int fibonacciSearch(const struct Student a[], int n, int key) {
    int fib2 = 0, fib1 = 1, fib = 1;
    int offset = -1, i;

    while (fib < n) {                  /* find smallest Fibonacci >= n */
        fib2 = fib1;
        fib1 = fib;
        fib = fib2 + fib1;
    }

    while (fib > 1) {
        i = (offset + fib2 < n - 1) ? offset + fib2 : n - 1;
        searchComparisons++;
        if (a[i].id < key) {           /* key is in the right part: move down one Fibonacci */
            fib = fib1;
            fib1 = fib2;
            fib2 = fib - fib1;
            offset = i;
        } else if (a[i].id > key) {    /* key is in the left part: move down two Fibonacci */
            fib = fib2;
            fib1 = fib1 - fib2;
            fib2 = fib - fib1;
        } else {
            return i;
        }
    }

    if (fib1 && offset + 1 < n) {      /* one element may remain */
        searchComparisons++;
        if (a[offset + 1].id == key) return offset + 1;
    }
    return -1;
}

/* Returns 1 if IDs are in ascending order, else 0. */
int isSortedById(const struct Student a[], int n) {
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1].id > a[i].id) return 0;
    }
    return 1;
}

/* Copies src into dest and sorts dest by ID (insertion sort).
   The original array is never touched. */
void sortCopyById(const struct Student src[], struct Student dest[], int n) {
    int i, j;
    struct Student temp;

    for (i = 0; i < n; i++) dest[i] = src[i];

    for (i = 1; i < n; i++) {
        temp = dest[i];
        j = i - 1;
        while (j >= 0 && dest[j].id > temp.id) {
            dest[j + 1] = dest[j];
            j--;
        }
        dest[j + 1] = temp;
    }
}

static void printStudent(const struct Student *s) {
    printf("\nStudent Found!\n\n");
    printf("ID        : %d\n", s->id);
    printf("Name      : %s\n", s->name);
    printf("Branch    : %s\n", s->branch);
    printf("Semester  : %d\n", s->semester);
    printf("Marks     : %.1f\n", s->marks);
    if (s->rank > 0) printf("Rank      : %d\n", s->rank);
    else             printf("Rank      : Not ranked yet\n");
}

void searchStudent(struct Student students[], int count) {
    struct Student sortedCopy[MAX_STUDENTS];
    struct Student *data = students;   /* array actually searched */
    int choice, key, index;

    if (count == 0) {
        printf("No records to search. Add students first.\n");
        return;
    }

    printf("\n1. Linear Search\n2. Sentinel Search\n3. Binary Search\n4. Fibonacci Search\n");
    choice = readInt("Choose search algorithm: ", 1, 4);
    key = readInt("Enter Student ID to search: ", 1, 999999);

    /* Binary and Fibonacci need sorted data: search a sorted COPY if needed */
    if ((choice == 3 || choice == 4) && !isSortedById(students, count)) {
        printf("Data is not sorted by ID. Searching a sorted temporary copy "
               "(original order unchanged).\n");
        sortCopyById(students, sortedCopy, count);
        data = sortedCopy;
    }

    searchComparisons = 0;
    switch (choice) {
        case 1: index = linearSearch(data, count, key); break;
        case 2: index = sentinelSearch(data, count, key); break;
        case 3: index = binarySearch(data, count, key); break;
        default: index = fibonacciSearch(data, count, key); break;
    }

    if (index >= 0) printStudent(&data[index]);
    else            printf("\nStudent not found.\n");
    printf("Comparisons made: %ld\n", searchComparisons);
}