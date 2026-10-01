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