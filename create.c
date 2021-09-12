/* create.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main() {

	int i, id;
	struct StudentInfo *infoptr;
	int sema_set;

	id = shmget(STU_KEY, STU_SEGSIZE, IPC_CREAT|0666); // get shared memory to store data
	if (id < 0) {
    perror("Create: shmget failed");
		exit(1);
	}

	// attach the shared memory segment to the process's address space
	infoptr=(struct StudentInfo *)shmat(id, 0, 0);
	if (infoptr <= (struct StudentInfo *) (0)) {
		perror("Create: shmat failed");
		exit(2);
  	}

	// get a set of NUM_SEMAPHS semaphores
	sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
	if (sema_set < 0) {
		perror("Create: semget failed");
		exit(2);
	}

	// store data in the shared memory segment
	strcpy(infoptr->Name,"Joe");
	//strcpy(infoptr->lName, "Smith");
	strcpy(infoptr->Phone, "606-111-2222");
	strcpy(infoptr->whoModified, " ");

	// print the contents of the shared memory segment 10 times
	for(i = 0; i < 10; i++) {
		printf("the value of sema_set=%d\n", sema_set);
		Wait(sema_set, 1);
		printf("Name: %s \nPhone Number: %s\n", infoptr->Name, infoptr->Phone);
		printf("Last modified by: %s\n \n ", infoptr->whoModified);
		sleep(2);
		Signal(sema_set, 1);
	}

	shmdt((char *)infoptr); // detach the shared memory segment
	shmctl(id, IPC_RMID, (struct shmid_ds *)0); // destroy the shared memory segment
	semctl(sema_set, 0, IPC_RMID); // remove the semaphore set
	exit(0);

}
