#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "student.h"

void compareSortingAlgorithms(const struct Student students[], int count);
void compareSearchingAlgorithms(const struct Student students[], int count);
void displayComplexityTable(void);
void algorithmComparisonMenu(const struct Student students[], int count);

#endif