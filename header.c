/* header.c */

/* This file, with the exception of GetAndCheckPassword(),
   was provided as sample code for the assignment, and is otherwise unmodified. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

// Lock the semaphore n of the semaphore set semaph
void Wait(int semaph, int n) {
	struct sembuf sop;
	sop.sem_num = n;
	sop.sem_op = -1;
	sop.sem_flg = 0;
	semop(semaph, &sop, 1);
}

// Unlock the semaphore n of the semaphore set semaph
void Signal(int semaph, int n) {
	struct sembuf sop;
	sop.sem_num = n;
	sop.sem_op = 1;
	sop.sem_flg = 0;
	semop(semaph, &sop, 1);
}

// make an array of n semaphores with key k
int GetSemaphs(key_t k, int n) {
	int semid, i;

	// get a set of n semaphores with the given key k
	if ((semid = semget(k, n, IPC_CREAT|0666)) != -1) {
		for (i = 0; i < n; i++) {
			Signal(semid, i); // unlock all the semaphores
    }
	}
	return semid;
}

// prompts user for advisor access password
// exits with code 3 if user enters incorrect password, else returns 1
int GetAndCheckPassword() {
	printf("PASSWORD: ");

	char password[50];
    fgets(password, 51, stdin);
    password[strlen(password) - 1] = '\0'; // trim newline

    if (strcmp(password, PASSWORD) != 0) {
        printf("INCORRECT PASSWORD. ACCESS DENIED.\n");
        exit(3);
    }
	return 1;
}
