#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 1) {
        fprintf(stderr, "Usage: ./print\n");
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

    Wait(semaset, 1);
    *read_count += 1;
    if (*read_count == 1) {
        Wait(semaset, 0);
    }
    Signal(semaset, 1);

    while (1) {
        printf("Name:       %s\n", student->Name);
        printf("Student ID: %s\n", student->StuID);
        printf("Address:    %s\n", student->Address);
        printf("Phone:      %s\n", student->Phone);
        student++;
        if ((int)strlen(student->Name) != 0) {
            printf("--------------------------------------------------\n");
        } else {
            break;
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