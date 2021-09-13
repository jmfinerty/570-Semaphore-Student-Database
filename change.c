/* change.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./Change <Student ID>\n");
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
    while (1) {
		if ((int)strlen(student->Name) == 0)
            break;
        if (strcmp(query, student->StuID) == 0) {

			printf("CURRENT STUDENT RECORD\n");
            printf("Name:       %s\n", student->Name);
            printf("Student ID: %s\n", student->StuID);
            printf("Address:    %s\n", student->Address);
            printf("Phone:      %s\n", student->Phone);

			int choice = -1;
			while (choice != 0) {
				printf("\nSelect a value to change.\n");
				printf("\t1. Student Name\n\t2. Student ID\n\t3. Address\n\t4. Phone\n\t0. EXIT\n");
				printf("Choice: ");

				char buf[1];
				fgets(buf, 3, stdin);
				choice = atoi(buf);

				switch (choice) {
					case 1:
						printf("New name: 		");
						fgets(student->Name, 51, stdin);
						student->Name[strlen(student->Name) - 1] = '\0';
						break;
					case 2:
						printf("New Student ID:	");
						fgets(student->StuID, 26, stdin);
						student->StuID[strlen(student->StuID) - 1] = '\0';
						break;
					case 3:
						printf("New Address: 	");
						fgets(student->Address, 76, stdin);
						student->Address[strlen(student->Address) - 1] = '\0';
						break;
					case 4:
						printf("New Phone: 		");
						fgets(student->Phone, 26, stdin);
						student->Phone[strlen(student->Phone) - 1] = '\0';
						break;
					case 0:
						exit(3);
					default:
						printf("Invalid selection.");
						break;
				}
			}
            num_records_found++;
        }
        student++;
    }

    if (num_records_found == 0) {
        printf("No student with this ID found in the database.\n");
    }

    Wait(semaset, 1);
    *read_count -= 1;
    if (*read_count == 0) {
        Signal(semaset, 0);
    }
    Signal(semaset, 1);

    return 0;

}