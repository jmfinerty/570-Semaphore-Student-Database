#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./load <filename>\n");
        exit(3);
    }

    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("Load: shmget failed");
		exit(1);
	}

    struct StudentInfo* student = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (student <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("Load: shmat failed");
		exit(2);
    }

    int semaset = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
    if (semaset < 0) {
		perror("Load: semget failed");
		exit(2);
	}

    FILE* f = fopen(argv[1], "r");
    if (f) {
        Wait(semaset, 0);
        *read_count = 0;

        while (!feof(f)) {
            fgets(student->Name, 51, f);
            if ((int)strlen(student->Name) == 0)
                break;
            fgets(student->StuID, 26, f);
            fgets(student->Address, 76, f);
            fgets(student->Phone, 26, f);

            // remove newlines
            student->Name[strlen(student->Name) - 1] = '\0';
            student->StuID[strlen(student->StuID) - 1] = '\0';
            student->Address[strlen(student->Address) - 1] = '\0';
            student->Phone[strlen(student->Phone) - 1] = '\0';

            printf("Loaded: %s\n", student->Name);
            student++;
        }

        Signal(semaset, 0);

    } else {
        perror("Load: Error reading file.");
        exit(3);
    }

    return 0;

}