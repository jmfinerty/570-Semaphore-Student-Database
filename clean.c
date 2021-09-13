#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    // Check for correct usage
    if (argc != 1) {
        fprintf(stderr, "Usage: ./Clean\n");
        exit(3);
    }

    // Prompt user for password to enable use
    char password[50];
    printf("PASSWORD: ");
    fgets(password, 51, stdin);
    password[strlen(password) - 1] = '\0';
    if (strcmp(password, PASSWORD) != 0) {
        printf("INCORRECT PASSWORD. ACCESS DENIED.\n");
        exit(3);
    }

    // Get shared memory segment IDs
    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("CLEAN: shmget failed");
		exit(1);
	}

    // Attach shared memory segments
    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("CLEAN: shmat failed");
		exit(2);
    }

    // Get semaset identifier
    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) {
		perror("CLEAN: semget failed");
		exit(2);
	}

    Wait(semaset, 0);

    FILE* f = fopen("clean_output.txt", "w");
    if (f) {
        while (1) {

            // Stop after all students have been checked
            if ((int)strlen(students->Name) == 0)
                break;

            // Write each student's entry out to file
            fputs(students->Name, f);
            fputs(students->StuID, f);
            fputs(students->Address, f);
            fputs(students->Phone, f);
            printf("Wrote out: %s\n", students->Name);
            students++;

            if (ENABLE_TESTING_SLEEP) {
                sleep(TESTING_SLEEP_LENGTH);
            }
        }
    } else {
        perror("CLEAN: Error reading file.");
        exit(3);
    }

    // Detach shared memory segments
    shmdt((char*)students);
    shmdt((char*)read_count);

    // Mark segments to be destroyed
    shmctl(stu_id, IPC_RMID, (struct shmid_ds*)0);
    shmctl(reads_id, IPC_RMID, (struct shmid_ds*)0);

    Signal(semaset, 0);
    semctl(semaset, 0, IPC_RMID);

    return 0;

}
