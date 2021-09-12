/* header.h */

#define KEY ((key_t)(07438)) // changed to last five digits of my student ID
#define SEGSIZE	sizeof(struct StudentInfo)

#define NUM_SEMAPHS 5
#define SEMA_KEY ((key_t)(7438)) // changed this to last four digits of my student ID

struct StudentInfo{
	char fName[20];
	char lName[20];
	char telNumber[15];
	char whoModified[10];
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);
