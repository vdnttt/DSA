#include <stdio.h>
#include <string.h>
#include "student.h"
#include "sorting.h"

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