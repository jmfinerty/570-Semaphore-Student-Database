#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./query <Student ID>\n");
        exit(3);
    }

    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("Query: shmget failed");
		exit(1);
	}

    struct StudentInfo* student = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (student <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("Query: shmat failed");
		exit(2);
    }

    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) {
		perror("Query: semget failed");
		exit(2);
	}

    char* query = argv[1];

    Wait(semaset, 1);
    *read_count += 1;
    if (*read_count == 1) {
        Wait(semaset, 0);
    }
    Signal(semaset, 1);

    int num_records_found = 0;
    for (int i=0; i<10; i++) { // TODO, figure out how to do this
        if (strcmp(query, student->StuID) == 0) {
            printf("Name:       %s\n", student->Name);
            printf("Student ID: %s\n", student->StuID);
            printf("Address:    %s\n", student->Address);
            printf("Phone:      %s\n", student->Phone);
            num_records_found++;
        }
        student++;
    }

    if (num_records_found == 0) {
        printf("No student with this ID found in the database.\n");
    }

    sleep(2);
    Wait(semaset, 1);
    *read_count -= 1;
    if (*read_count == 0) {
        Signal(semaset, 0);
    }
    Signal(semaset, 1);

    return 0;

}