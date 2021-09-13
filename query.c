#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    // Check for correct usage
    if (argc != 2) {
        fprintf(stderr, "Usage: ./Query <Student ID>\n");
        exit(3);
    }

    // Get shared memory segment IDs
    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("QUERY: shmget failed");
		exit(1);
	}

    // Attach shared memory segments
    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("QUERY: shmat failed");
		exit(2);
    }

    // Get semaset identifier
    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) {
		perror("QUERY: semget failed");
		exit(2);
	}

    char* query = argv[1];

    Wait(semaset, 1);
    *read_count += 1;
    if (*read_count == 1) {
        Wait(semaset, 0);
    }
    Signal(semaset, 1);

    // Check each student record for a matching ID
    int num_records_found = 0;
    while (1) {

        if (strcmp(query, students->StuID) == 0) {
            // When a matching ID is found, print that student's record
            printf("Name:       %s\n", students->Name);
            printf("Student ID: %s\n", students->StuID);
            printf("Address:    %s\n", students->Address);
            printf("Phone:      %s\n", students->Phone);
            num_records_found++;
        }
        students++;

        // Stop after last student has been read
        if ((int)strlen(students->Name) == 0)
            break;

        if (ENABLE_TESTING_SLEEP) {
            sleep(TESTING_SLEEP_LENGTH);
        }
    }

    if (num_records_found == 0) {
        printf("No students with this ID found in the database.\n");
    }

    Wait(semaset, 1);
    *read_count -= 1;
    if (*read_count == 0) {
        Signal(semaset, 0);
    }
    Signal(semaset, 1);

    return 0;

}