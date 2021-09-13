#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./Load <filename>\n");
        exit(3);
    }

    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("LOAD: shmget failed");
		exit(1);
	}

    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("LOAD: shmat failed");
		exit(2);
    }

    int semaset = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
    if (semaset < 0) {
		perror("LOAD: semget failed");
		exit(2);
	}

    FILE* f = fopen(argv[1], "r");
    if (f) {
        Wait(semaset, 0);
        *read_count = 0;

        while (!feof(f)) {
            fgets(students->Name, 51, f);
            if ((int)strlen(students->Name) == 0)
                break;
            fgets(students->StuID, 26, f);
            fgets(students->Address, 76, f);
            fgets(students->Phone, 26, f);

            // remove newlines
            students->Name[strlen(students->Name) - 1] = '\0';
            students->StuID[strlen(students->StuID) - 1] = '\0';
            students->Address[strlen(students->Address) - 1] = '\0';
            students->Phone[strlen(students->Phone) - 1] = '\0';

            printf("Loaded: %s\n", students->Name);

            if (ENABLE_TESTING_SLEEP) {
                sleep(TESTING_SLEEP_LENGTH);
            }

            students++;
        }

        Signal(semaset, 0);

    } else {
        perror("LOAD: Error reading file.");
        exit(3);
    }

    return 0;

}