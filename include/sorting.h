#ifndef SORTING_H
#define SORTING_H

#include "student.h"

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