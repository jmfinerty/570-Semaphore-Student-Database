/* clean.c */

/* Lets a student advisor save the date stored in the shared memory to a file, in a format that is suitable
for loading into memory later and also deletes the shared memory segments and the semaphores created
for synchronization. The “Clean” program should prompt the user (advisor) to type in a password;
the password is “000”. */

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

    GetAndCheckPassword();

    // Get shared memory segment IDs
    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) { // return value -1 indicates an error, see man: https://man7.org/linux/man-pages/man2/shmget.2.html
		perror("CLEAN: shmget failed");
		exit(1);
	}

    // Attach shared memory segments
    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {  // return value -1 indicates an error, see man: https://man7.org/linux/man-pages/man3/shmat.3p.html
		perror("CLEAN: shmat failed");
		exit(2);
    }

    // Get semaset identifier
    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) { // return value -1 indicates an error, see man: https://man7.org/linux/man-pages/man2/semget.2.html
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
            fputs("\n", f);
            fputs(students->StuID, f);
            fputs("\n", f);
            fputs(students->Address, f);
            fputs("\n", f);
            fputs(students->Phone, f);
            fputs("\n", f);
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
