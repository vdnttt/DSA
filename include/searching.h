#ifndef SEARCHING_H
#define SEARCHING_H

#include "student.h"

extern long searchComparisons;   /* key comparisons made by the last search */

int linearSearch(const struct Student a[], int n, int key);
int sentinelSearch(struct Student a[], int n, int key);
int binarySearch(const struct Student a[], int n, int key);      /* needs sorted-by-ID array */
int fibonacciSearch(const struct Student a[], int n, int key);   /* needs sorted-by-ID array */

int  isSortedById(const struct Student a[], int n);
void sortCopyById(const struct Student src[], struct Student dest[], int n);
void searchStudent(struct Student students[], int count);

#endif