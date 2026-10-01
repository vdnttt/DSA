#ifndef STUDENT_H
#define STUDENT_H

#define MAX_STUDENTS 100   /* fixed array size: no dynamic memory */

/* One student record. An array of these is our only data storage. */
struct Student {
    int id;
    char name[50];
    char branch[50];
    int semester;
    float marks;
    int rank;          /* filled in later by generateRanking() */
};

/* Input helpers (reused by menus in other modules) */
int   readInt(const char *prompt, int min, int max);
float readFloat(const char *prompt, float min, float max);

/* Record operations */
int  idExists(const struct Student students[], int count, int id);
void addStudents(struct Student students[], int *count);
void displayStudents(const struct Student students[], int count);

void assignRanks(struct Student students[], int count);
void generateRanking(struct Student students[], int count);
void displayTopStudents(struct Student students[], int count);

#endif
#ifndef SEARCHING_H
#define SEARCHING_H


extern long searchComparisons;   /* key comparisons made by the last search */

int linearSearch(const struct Student a[], int n, int key);
int sentinelSearch(struct Student a[], int n, int key);
int binarySearch(const struct Student a[], int n, int key);      /* needs sorted-by-ID array */
int fibonacciSearch(const struct Student a[], int n, int key);   /* needs sorted-by-ID array */

int  isSortedById(const struct Student a[], int n);
void sortCopyById(const struct Student src[], struct Student dest[], int n);
void searchStudent(struct Student students[], int count);

#endif
#ifndef SORTING_H
#define SORTING_H


#define SORT_BY_MARKS 1
#define SORT_BY_ID    2
#define SORT_BY_NAME  3

extern long sortComparisons;   /* comparisons made by the last sort */
extern long sortSwaps;         /* swaps / element movements */

void bubbleSort(struct Student a[], int n, int field);
void selectionSort(struct Student a[], int n, int field);
void insertionSort(struct Student a[], int n, int field);
void shellSort(struct Student a[], int n, int field);
void mergeSort(struct Student a[], int n, int field);
void quickSort(struct Student a[], int n, int field);
void radixSort(struct Student a[], int n, int field);    /* marks or ID only */
void bucketSort(struct Student a[], int n, int field);   /* marks or ID only */

void sortStudents(struct Student students[], int count); /* menu option 4 */

#endif
#ifndef ANALYSIS_H
#define ANALYSIS_H


void compareSortingAlgorithms(const struct Student students[], int count);
void compareSearchingAlgorithms(const struct Student students[], int count);
void displayComplexityTable(void);
void algorithmComparisonMenu(const struct Student students[], int count);

#endif
#include <stdio.h>
#include <string.h>

/* Read an integer within [min, max]. Repeats until the input is valid.
   We read a whole line with fgets and then parse it with sscanf, which
   avoids the leftover-newline problems that scanf causes. */
int readInt(const char *prompt, int min, int max) {
    char line[50];
    int value;

    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) != NULL &&
            sscanf(line, "%d", &value) == 1 &&
            value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Enter a whole number from %d to %d.\n", min, max);
    }
}

/* Same idea for decimal numbers (used for marks). */
float readFloat(const char *prompt, float min, float max) {
    char line[50];
    float value;

    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) != NULL &&
            sscanf(line, "%f", &value) == 1 &&
            value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Enter a number from %.0f to %.0f.\n", min, max);
    }
}

/* Read a non-empty line of text (for name and branch). */
static void readText(const char *prompt, char *buffer, int size) {
    do {
        printf("%s", prompt);
        if (fgets(buffer, size, stdin) == NULL) {
            buffer[0] = '\0';
        }
        buffer[strcspn(buffer, "\n")] = '\0';   /* remove trailing newline */
        if (buffer[0] == '\0') {
            printf("This field cannot be empty.\n");
        }
    } while (buffer[0] == '\0');
}

/* Returns 1 if the ID is already stored, otherwise 0.
   This is a simple linear scan over the used part of the array. */
int idExists(const struct Student students[], int count, int id) {
    int i;
    for (i = 0; i < count; i++) {
        if (students[i].id == id) {
            return 1;
        }
    }
    return 0;
}

void addStudents(struct Student students[], int *count) {
    int remaining = MAX_STUDENTS - *count;
    int howMany, i;

    if (remaining == 0) {
        printf("Storage is full (%d students). Cannot add more.\n", MAX_STUDENTS);
        return;
    }

    printf("You can add up to %d more student(s).\n", remaining);
    howMany = readInt("How many students do you want to add? ", 1, remaining);

    for (i = 0; i < howMany; i++) {
        struct Student s;

        printf("\n--- Student %d of %d ---\n", i + 1, howMany);

        /* IDs must be unique so that searching by ID makes sense */
        do {
            s.id = readInt("Student ID (positive number): ", 1, 999999);
            if (idExists(students, *count, s.id)) {
                printf("That ID already exists. Enter a different ID.\n");
            }
        } while (idExists(students, *count, s.id));

        readText("Name: ", s.name, sizeof(s.name));
        readText("Branch: ", s.branch, sizeof(s.branch));
        s.semester = readInt("Semester (1-8): ", 1, 8);
        s.marks = readFloat("Marks (0-100): ", 0.0f, 100.0f);
        s.rank = 0;                    /* not ranked yet */

        students[*count] = s;          /* copy the whole struct into the next free slot */
        (*count)++;                    /* one more slot is now in use */
    }
    for (i = 0; i < *count; i++) students[i].rank = 0;   /* ranks are now out of date */
    printf("\n%d student(s) added. Total records: %d\n", howMany, *count);
}

void displayStudents(const struct Student students[], int count) {
    int i;

    if (count == 0) {
        printf("No student records to display.\n");
        return;
    }

    printf("\n-----------------------------------------------------------------------\n");
    printf("%-8s %-17s %-12s %-13s %s\n", "ID", "Name", "Branch", "Semester", "Marks");
    printf("-----------------------------------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("%-8d %-17s %-12s %-13d %.1f\n",
               students[i].id, students[i].name, students[i].branch,
               students[i].semester, students[i].marks);
    }
    printf("-----------------------------------------------------------------------\n");
}

/* RANKING LOGIC (standard competition ranking):
   1. Sort by marks, highest first, with merge sort (stable, so tied students
      keep their entry order: this is our tie-breaking rule).
   2. Walk the sorted array once:
        first student            -> rank 1
        marks == previous marks  -> same rank as previous
        otherwise                -> rank = position + 1
   Example: 95 92 92 85 78  ->  1 2 2 4 5
   Time O(n log n) for the sort + O(n) for the walk. */
void assignRanks(struct Student students[], int count) {
    int i;
    if (count == 0) return;

    mergeSort(students, count, SORT_BY_MARKS);

    students[0].rank = 1;
    for (i = 1; i < count; i++) {
        if (students[i].marks == students[i - 1].marks) {
            students[i].rank = students[i - 1].rank;   /* tie: share the rank */
        } else {
            students[i].rank = i + 1;                  /* skips ranks after a tie */
        }
    }
}

static void printRankTable(const struct Student a[], int n) {
    int i;
    printf("\n%-6s %-8s %-17s %-12s %s\n", "Rank", "ID", "Name", "Branch", "Marks");
    printf("---------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-6d %-8d %-17s %-12s %.1f\n",
               a[i].rank, a[i].id, a[i].name, a[i].branch, a[i].marks);
    }
    printf("---------------------------------------------------------\n");
}

void generateRanking(struct Student students[], int count) {
    if (count == 0) {
        printf("No records to rank. Add students first.\n");
        return;
    }
    assignRanks(students, count);
    printRankTable(students, count);
    printf("Ranking generated. Students with equal marks share a rank.\n");
}

void displayTopStudents(struct Student students[], int count) {
    int choice, limit;

    if (count == 0) {
        printf("No records available. Add students first.\n");
        return;
    }

    printf("\n1. Top 3\n2. Top 5\n3. Top 10\n");
    choice = readInt("Choose: ", 1, 3);
    limit = (choice == 1) ? 3 : (choice == 2) ? 5 : 10;

    if (limit > count) {
        printf("Only %d student(s) available, showing all of them.\n", count);
        limit = count;
    }

    assignRanks(students, count);      /* make sure ranks are current */
    printRankTable(students, limit);   /* array is in rank order: first N rows */
}
#include <stdio.h>

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
#include <stdio.h>
#include <ctype.h>

#define NUM_BUCKETS 10
#define MARKS_SCALE_MAX 10000   /* marks 0..100 become integers 0..10000 */

long sortComparisons = 0;
long sortSwaps = 0;

/* ---------- helpers ---------- */

/* Case-insensitive string compare (negative, 0, positive like strcmp). */
static int compareNames(const char *x, const char *y) {
    while (*x && *y && tolower((unsigned char)*x) == tolower((unsigned char)*y)) {
        x++;
        y++;
    }
    return tolower((unsigned char)*x) - tolower((unsigned char)*y);
}

/* The ONE place where order is decided. Returns <0 if x must come before y,
   0 if equal, >0 if x must come after y.
   Marks: descending (higher marks first). ID and Name: ascending.
   Every call counts as one comparison. */
static int compareStudents(const struct Student *x, const struct Student *y, int field) {
    sortComparisons++;
    if (field == SORT_BY_MARKS) {
        if (x->marks > y->marks) return -1;
        if (x->marks < y->marks) return 1;
        return 0;
    }
    if (field == SORT_BY_ID) return x->id - y->id;
    return compareNames(x->name, y->name);
}

static void swapStudents(struct Student *x, struct Student *y) {
    struct Student t = *x;
    *x = *y;
    *y = t;
    sortSwaps++;
}

/* Integer key for radix/bucket sort. Smaller key = comes earlier.
   For marks we subtract from the maximum so descending marks become ascending keys. */
static int getKey(const struct Student *s, int field) {
    if (field == SORT_BY_ID) return s->id;
    return MARKS_SCALE_MAX - (int)(s->marks * 100 + 0.5f);
}

/* ---------- comparison-based sorts ---------- */

/* BUBBLE SORT: repeatedly swap adjacent out-of-order pairs; the largest
   "bubbles" to the end each pass. Stops early if a pass makes no swap.
   Best O(n) (already sorted), Average/Worst O(n^2). Space O(1). */
void bubbleSort(struct Student a[], int n, int field) {
    int i, j, swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            if (compareStudents(&a[j], &a[j + 1], field) > 0) {
                swapStudents(&a[j], &a[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* SELECTION SORT: find the smallest remaining element and swap it into place.
   Always O(n^2) comparisons, but at most n-1 swaps. Space O(1). */
void selectionSort(struct Student a[], int n, int field) {
    int i, j, min;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++) {
            if (compareStudents(&a[j], &a[min], field) < 0) min = j;
        }
        if (min != i) swapStudents(&a[i], &a[min]);
    }
}

/* INSERTION SORT: take the next element and shift larger ones right to make room.
   Best O(n), Average/Worst O(n^2). Space O(1). Good for nearly sorted data. */
void insertionSort(struct Student a[], int n, int field) {
    int i, j;
    struct Student key;
    for (i = 1; i < n; i++) {
        key = a[i];
        j = i - 1;
        while (j >= 0 && compareStudents(&a[j], &key, field) > 0) {
            a[j + 1] = a[j];           /* shift right */
            sortSwaps++;
            j--;
        }
        if (j + 1 != i) {
            a[j + 1] = key;
            sortSwaps++;
        }
    }
}

/* SHELL SORT: insertion sort on elements 'gap' apart, with the gap halving
   (n/2, n/4, ... 1). Far-apart elements move quickly. With this gap sequence
   the worst case is O(n^2); it is often much faster in practice. Space O(1). */
void shellSort(struct Student a[], int n, int field) {
    int gap, i, j;
    struct Student key;
    for (gap = n / 2; gap > 0; gap /= 2) {
        for (i = gap; i < n; i++) {
            key = a[i];
            j = i;
            while (j >= gap && compareStudents(&a[j - gap], &key, field) > 0) {
                a[j] = a[j - gap];
                sortSwaps++;
                j -= gap;
            }
            if (j != i) {
                a[j] = key;
                sortSwaps++;
            }
        }
    }
}

/* MERGE SORT: divide the array in half, sort each half, merge them.
   O(n log n) in all cases. Needs an extra array of size n: space O(n).
   Movements counted = elements copied into the temporary array. */
static struct Student mergeTemp[MAX_STUDENTS];

static void mergeParts(struct Student a[], int low, int mid, int high, int field) {
    int i = low, j = mid + 1, k = low;

    while (i <= mid && j <= high) {
        /* <= keeps equal elements in original order (stable) */
        if (compareStudents(&a[i], &a[j], field) <= 0) mergeTemp[k++] = a[i++];
        else                                           mergeTemp[k++] = a[j++];
        sortSwaps++;
    }
    while (i <= mid)  { mergeTemp[k++] = a[i++]; sortSwaps++; }
    while (j <= high) { mergeTemp[k++] = a[j++]; sortSwaps++; }

    for (k = low; k <= high; k++) a[k] = mergeTemp[k];
}

static void mergeSortRange(struct Student a[], int low, int high, int field) {
    int mid;
    if (low >= high) return;           /* 0 or 1 element: already sorted */
    mid = low + (high - low) / 2;
    mergeSortRange(a, low, mid, field);
    mergeSortRange(a, mid + 1, high, field);
    mergeParts(a, low, mid, high, field);
}

void mergeSort(struct Student a[], int n, int field) {
    mergeSortRange(a, 0, n - 1, field);
}

/* QUICK SORT: pick a pivot (last element), move smaller elements before it and
   larger after it (partition), then sort both sides recursively.
   Best/Average O(n log n), Worst O(n^2) (e.g. already sorted with this pivot).
   Space O(log n) for recursion on average. */
static int partition(struct Student a[], int low, int high, int field) {
    int i = low - 1, j;
    for (j = low; j < high; j++) {
        if (compareStudents(&a[j], &a[high], field) < 0) {
            i++;
            if (i != j) swapStudents(&a[i], &a[j]);
        }
    }
    if (i + 1 != high) swapStudents(&a[i + 1], &a[high]);
    return i + 1;                       /* final position of the pivot */
}

static void quickSortRange(struct Student a[], int low, int high, int field) {
    int p;
    if (low >= high) return;
    p = partition(a, low, high, field);
    quickSortRange(a, low, p - 1, field);
    quickSortRange(a, p + 1, high, field);
}

void quickSort(struct Student a[], int n, int field) {
    quickSortRange(a, 0, n - 1, field);
}

/* ---------- non-comparison sorts (numeric keys only) ---------- */

/* RADIX SORT (LSD): sort by the ones digit, then tens, then hundreds...
   using a stable counting sort for each digit. Never compares two elements,
   so sortComparisons stays 0. O(d * (n + 10)) where d = number of digits.
   Space O(n). Movements counted = elements placed into the output array. */
void radixSort(struct Student a[], int n, int field) {
    static struct Student output[MAX_STUDENTS];
    int count[10];
    int maxKey = 0, exp, i, d;

    for (i = 0; i < n; i++) {
        if (getKey(&a[i], field) > maxKey) maxKey = getKey(&a[i], field);
    }

    for (exp = 1; maxKey / exp > 0; exp *= 10) {
        for (d = 0; d < 10; d++) count[d] = 0;

        for (i = 0; i < n; i++) count[(getKey(&a[i], field) / exp) % 10]++;

        for (d = 1; d < 10; d++) count[d] += count[d - 1];   /* prefix sums = end positions */

        for (i = n - 1; i >= 0; i--) {                       /* backwards keeps it stable */
            d = (getKey(&a[i], field) / exp) % 10;
            output[--count[d]] = a[i];
            sortSwaps++;
        }

        for (i = 0; i < n; i++) a[i] = output[i];
    }
}

/* BUCKET SORT: spread elements into 10 buckets by key range, sort each bucket
   with insertion sort, then read the buckets in order.
   Buckets are a fixed 2D array (no linked lists).
   Average O(n + k) for evenly spread keys, Worst O(n^2). Space O(n + k).
   Comparisons counted = those made inside the bucket insertion sorts. */
void bucketSort(struct Student a[], int n, int field) {
    static struct Student bucket[NUM_BUCKETS][MAX_STUDENTS];
    int size[NUM_BUCKETS] = {0};
    int maxKey = 0, i, b, j, k;
    struct Student temp;

    if (n == 0) return;

    for (i = 0; i < n; i++) {
        if (getKey(&a[i], field) > maxKey) maxKey = getKey(&a[i], field);
    }

    /* 1. distribute: bucket number = key scaled into 0..NUM_BUCKETS-1 */
    for (i = 0; i < n; i++) {
        b = (int)((long)getKey(&a[i], field) * NUM_BUCKETS / (maxKey + 1));
        bucket[b][size[b]++] = a[i];
        sortSwaps++;
    }

    /* 2. sort each bucket with insertion sort */
    for (b = 0; b < NUM_BUCKETS; b++) {
        for (i = 1; i < size[b]; i++) {
            temp = bucket[b][i];
            j = i - 1;
            while (j >= 0) {
                sortComparisons++;
                if (getKey(&bucket[b][j], field) > getKey(&temp, field)) {
                    bucket[b][j + 1] = bucket[b][j];
                    sortSwaps++;
                    j--;
                } else {
                    break;
                }
            }
            if (j + 1 != i) {
                bucket[b][j + 1] = temp;
                sortSwaps++;
            }
        }
    }

    /* 3. concatenate the buckets back into the array */
    k = 0;
    for (b = 0; b < NUM_BUCKETS; b++) {
        for (i = 0; i < size[b]; i++) a[k++] = bucket[b][i];
    }
}

/* ---------- menu ---------- */

static void printSortedTable(const struct Student a[], int n) {
    int i;
    printf("\n%-6s %-8s %-17s %-12s %s\n", "Rank", "ID", "Name", "Branch", "Marks");
    printf("---------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        if (a[i].rank > 0) printf("%-6d ", a[i].rank);
        else               printf("%-6s ", "-");      /* not ranked yet */
        printf("%-8d %-17s %-12s %.1f\n", a[i].id, a[i].name, a[i].branch, a[i].marks);
    }
    printf("---------------------------------------------------------\n");
}

void sortStudents(struct Student students[], int count) {
    int algo, field;

    if (count == 0) {
        printf("No records to sort. Add students first.\n");
        return;
    }

    printf("\n1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n4. Shell Sort\n");
    printf("5. Merge Sort\n6. Quick Sort\n7. Radix Sort\n8. Bucket Sort\n");
    algo = readInt("Choose sorting algorithm: ", 1, 8);

    printf("\nSort by:\n1. Marks (highest first)\n2. Student ID\n3. Name\n");
    field = readInt("Choose field: ", 1, 3);

    if ((algo == 7 || algo == 8) && field == SORT_BY_NAME) {
        printf("Radix and Bucket sort work on numbers only, so they cannot sort by name.\n");
        printf("Choose another algorithm, or sort by Marks or ID.\n");
        return;
    }

    sortComparisons = 0;
    sortSwaps = 0;
    switch (algo) {
        case 1: bubbleSort(students, count, field); break;
        case 2: selectionSort(students, count, field); break;
        case 3: insertionSort(students, count, field); break;
        case 4: shellSort(students, count, field); break;
        case 5: mergeSort(students, count, field); break;
        case 6: quickSort(students, count, field); break;
        case 7: radixSort(students, count, field); break;
        default: bucketSort(students, count, field); break;
    }

    printf("\nRecords sorted (the stored order has changed).\n");
    printSortedTable(students, count);
    printf("Comparisons: %ld   Swaps/Movements: %ld\n", sortComparisons, sortSwaps);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



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
#include <stdio.h>

static void printMenu(void) {
    printf("\n==================================================\n");
    printf("       SMART STUDENT SEARCH & RANKING SYSTEM\n");
    printf("==================================================\n\n");
    printf("1. Add Student Records\n");
    printf("2. Display All Students\n");
    printf("3. Search Student\n");
    printf("4. Sort Students\n");
    printf("5. Generate Ranking\n");
    printf("6. Display Top Students\n");
    printf("7. Algorithm Comparison\n");
    printf("8. Exit\n\n");
}

int main(void) {
    struct Student students[MAX_STUDENTS];   /* the array of structures */
    int count = 0;                           /* number of records in use */
    int choice;

    do {
        printMenu();
        choice = readInt("Enter your choice: ", 1, 8);

        switch (choice) {
            case 1: addStudents(students, &count); break;
            case 2: displayStudents(students, count); break;
            case 3: searchStudent(students, count); break;
            case 4: sortStudents(students, count); break;
            case 5: generateRanking(students, count); break;
            case 6: displayTopStudents(students, count); break;
            case 7: algorithmComparisonMenu(students, count); break;
            case 8: printf("Goodbye!\n"); break;
        }
    } while (choice != 8);

    return 0;
}
