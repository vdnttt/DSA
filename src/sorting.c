#include <stdio.h>
#include <ctype.h>
#include "sorting.h"

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