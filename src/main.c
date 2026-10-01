#include <stdio.h>
#include "student.h"
#include "searching.h"
#include "sorting.h"
#include "analysis.h"

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