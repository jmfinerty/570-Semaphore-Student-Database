/* header.h */

// NOTE: The last 5 digits of my SID are 07438.
// Because this begins with 0, using this key will cause
//  error: invalid digit "8" in octal constant
// So, I have opted to use 17438.

// Students memory
#define KEYSTU ((key_t)(17438))
#define SEGSIZESTU sizeof(struct StudentInfo)

// Read count memory
#define KEYREAD ((key_t)(7438))
#define SEGSIZEREAD sizeof(int)

#define NUM_SEMAPHS 2
#define SEMA_KEY ((key_t)(438))

struct StudentInfo{
	char fName[20];
	char lName[20];
	char telNumber[15];
	char whoModified[10];
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);
