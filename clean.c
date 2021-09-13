#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    char password[50];
    printf("PASSWORD: ");
    fgets(password, 51, stdin);
    password[strlen(password) - 1] = '\0';
    if (strcmp(password, PASSWORD) != 0) {
        printf("INCORRECT PASSWORD. ACCESS DENIED.\n");
        exit(3);
    }

    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("Clean: shmget failed");
		exit(1);
	}

    struct StudentInfo* student = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (student <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("Clean: shmat failed");
		exit(2);
    }

    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) {
		perror("Clean: semget failed");
		exit(2);
	}

    Wait(semaset, 0);

    FILE* f = fopen("clean_output.txt", "w");
    if (f) {
        while (1) {
            if ((int)strlen(student->Name) == 0)
                break;
            fputs(student->Name, f);
            fputs(student->StuID, f);
            fputs(student->Address, f);
            fputs(student->Phone, f);
            printf("Wrote out: %s\n", student->Name);
            student++;

            if (ENABLE_TESTING_SLEEP) {
                sleep(TESTING_SLEEP_LENGTH);
            }
        }
    } else {
        perror("Load: Error reading file.");
        exit(3);
    }

    shmdt((char*)student);
    shmdt((char*)read_count);
    shmctl(stu_id, IPC_RMID, (struct shmid_ds*)0);
    shmctl(reads_id, IPC_RMID, (struct shmid_ds*)0);

    Signal(semaset, 0);
    semctl(semaset, 0, IPC_RMID);

    return 0;

}
