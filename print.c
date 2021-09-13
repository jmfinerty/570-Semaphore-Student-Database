#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 1) {
        fprintf(stderr, "Usage: ./Print\n");
        exit(3);
    }

    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("PRINT: shmget failed");
		exit(1);
	}

    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("PRINT: shmat failed");
		exit(2);
    }

    int semaset = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
    if (semaset < 0) {
		perror("PRINT: semget failed");
		exit(2);
	}

    Wait(semaset, 1);
    *read_count += 1;
    if (*read_count == 1) {
        Wait(semaset, 0);
    }
    Signal(semaset, 1);

    while (1) {
        if ((int)strlen(students->Name) == 0)
            break;
        printf("Name:       %s\n", students->Name);
        printf("Student ID: %s\n", students->StuID);
        printf("Address:    %s\n", students->Address);
        printf("Phone:      %s\n", students->Phone);
        students++;
        if ((int)strlen(students->Name) != 0)
            printf("--------------------------------------------------\n");

        if (ENABLE_TESTING_SLEEP) {
            sleep(TESTING_SLEEP_LENGTH);
        }
    }

    Wait(semaset, 1);
    *read_count -= 1;
    if (*read_count == 0) {
        Signal(semaset, 0);
    }
    Signal(semaset, 1);

    return 0;

}