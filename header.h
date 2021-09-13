/* header.h */

// NOTE: The last 5 digits of my SID are 07438.
// Because this begins with 0, using this key will cause
//  error: invalid digit "8" in octal constant
// So, I have opted to use 17438.

// Turn this to 0 to disable synchronicity testing sleeps
#define ENABLE_TESTING_SLEEP 1
#define TESTING_SLEEP_LENGTH 2 // time in seconds to sleep

// Advisor password
#define PASSWORD "000"

// Students memory
#define STU_KEY ((key_t)(17438))
#define STU_SEGSIZE sizeof(struct StudentInfo)

// Read count memory
#define READS_KEY ((key_t)(7438))
#define READS_SEGSIZE sizeof(int)

#define NUM_SEMAPHS 2
#define SEMA_KEY ((key_t)(438))

struct StudentInfo {
	char Name[50];
	char StuID[25];
	char Address[75];
	char Phone[25];
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);
