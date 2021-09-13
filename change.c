/* change.c */

/* Lets a student advisor change the data related to any student with the given Student ID. It also
allows an advisor to add/delete records to/from the database. A student advisor is required to use a
password to change the database. You can assume the password is “000”, for simplicity */


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
        fprintf(stderr, "Usage: ./Change <Student ID>\n");
        fprintf(stderr, "If you wish to add a new student, use the new student's Student ID.\n");
        exit(3);
    }

    GetAndCheckPassword();

    // Get shared memory segment IDs
    int stu_id       = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666);
    int reads_id     = shmget(READS_KEY, READS_SEGSIZE, IPC_CREAT|0666);
    if (stu_id < 0 || reads_id < 0) {
		perror("CHANGE: shmget failed");
		exit(1);
	}

    // Attach shared memory segments
    struct StudentInfo* students = (struct StudentInfo *)shmat(stu_id, 0, 0);
    int* read_count  = (int *)shmat(reads_id, 0, 0);
    if (students <= (struct StudentInfo *) (0) || read_count < (int *)(1)) {
		perror("CHANGE: shmat failed");
		exit(2);
    }

    // Get semaset identifier
    int semaset = semget(SEMA_KEY, 0, 0);
    if (semaset < 0) {
		perror("CHANGE: semget failed");
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

        // Stop after all students have been checked
		if ((int)strlen(students->Name) == 0)
            break;

        // Check for students with ID matching query
        if (strcmp(query, students->StuID) == 0) {

            // Print unmodified student record
			printf("CURRENT STUDENT RECORD\n");
            printf("Name:       %s\n", students->Name);
            printf("Student ID: %s\n", students->StuID);
            printf("Address:    %s\n", students->Address);
            printf("Phone:      %s\n", students->Phone);

			int choice = -1;
			while (choice != 0) {
				printf("\nSelect one of the following fields to modify.\n");
				printf("\t1. Student Name\n\t2. Student ID\n\t3. Address\n\t4. Phone\n\t9. DELETE STUDENT\n\t0. EXIT\n");
				printf("Choice: ");

                // Get user's choice for which field to modify
				char buf[1];
				fgets(buf, 3, stdin);
				choice = atoi(buf);

				switch (choice) {
					case 1: // 1. Student Name
						printf("New name: 		");
						fgets(students->Name, 51, stdin);
						students->Name[strlen(students->Name) - 1] = '\0';
						break;
					case 2: // 2. Student ID
						printf("New Student ID:	");
						fgets(students->StuID, 26, stdin);
						students->StuID[strlen(students->StuID) - 1] = '\0';
						break;
					case 3: // 3. Address
						printf("New Address: 	");
						fgets(students->Address, 76, stdin);
						students->Address[strlen(students->Address) - 1] = '\0';
						break;
					case 4: // 4. Phone
						printf("New Phone: 		");
						fgets(students->Phone, 26, stdin);
						students->Phone[strlen(students->Phone) - 1] = '\0';
						break;
                    case 9: // 9. Delete
                        while (1) {
                            struct StudentInfo* currentStudent = students;      // Save location of this students fields
                            students++;                                         // Move on to next student
                            strcpy(currentStudent->Name, students->Name);       // Copy next student over previous student
                            strcpy(currentStudent->StuID, students->StuID);
                            strcpy(currentStudent->Address, students->Address);
                            strcpy(currentStudent->Phone, students->Phone);

                            // When end of students is reached, the final student will
                            // be overwritten by empty strings, since that is what the
                            // student entry after them comprises. So, exit then.
                            if ((int)strlen(students->Name) == 0) {
                                printf("Student record deleted.");
                                exit(0);
                            }
                        }
					case 0: // 0. Exit
						exit(0);
					default:
						printf("Invalid selection.");
						break;
				}
			}
            num_records_found++;
        }
        students++;
    }

    // If no record was found, the user can enter a new record using the queried ID.
    // Note that we are already at the end of "students" from the above while loop,
    // and so we can just add to the fields in the position we are at.
    if (num_records_found == 0) {
        printf("No students with this ID found in the database. Add new student? (y/n): ");
        char str[10];
        fgets(str, 10, stdin);
        str[strlen(str) - 1] = '\0';

        if (strcmp(str, "y") == 0) {

            // copy student id from query
            strcpy(students->StuID, argv[1]);

            // get input for other fields
            printf("Name: 		");
            fgets(students->Name, 51, stdin);
            printf("Address: 	");
            fgets(students->Address, 76, stdin);
            printf("Phone: 		");
            fgets(students->Phone, 26, stdin);

            // trim newlines from fgets inputs
            students->Phone[strlen(students->Phone) - 1] = '\0';
            students->Name[strlen(students->Name) - 1] = '\0';
            students->Address[strlen(students->Address) - 1] = '\0';

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