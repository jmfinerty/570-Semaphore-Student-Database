# CS570, Modern Operating Systems, Assignment 1: "Interprocess communication through shared memory, Synchronization using semaphores"

See `hw1.pdf` for the specification of this assignment.

## Compilation
* Running `make` in the same directory as the `.c` files and `makefile` will compile the executables.
* Running `make clean` will remove the files produced by `make`.

## Usage

### Load
* Usage: `./Load <infile>`.
* `Load` will load the contents of the `infile` into the database.
* This file must be formatted as follows, and has only been tested with `.txt`.:
  ```
  Student 1 Name
  Student 1 StudentID
  Student 1 Address
  Student 1 Phone number
  Student 2 Name
  ...
  ```
 
### Print
* Usage: `./Print`.
* It will print the contents of the database in the following format:
  ```
  --------------------------------------------------
  Name:       Student 1 Name
  Student ID: Student 1 StudentID
  Address:    Student 1 Address
  Phone:      Student 1 Phone number
  --------------------------------------------------
  Name:       Student 2 Name
  Student ID: Student 2 StudentID
  Address:    Student 2 Address
  Phone:      Student 2 Phone number
  --------------------------------------------------
  ...
  ```
  
### Clean
* Usage: `./Clean`.
* `Clean` will write the contents of the database out to a file, then erase them from memory.
* Immediately after running, it will prompt for a password. This is hardcoded in `header.h` as `"000"`.

### Query
* Usage: `./Query <StudentID>`.
* `Query` will search the database for the student record with the given ID, then print the record.

### Change
* Usage: `./Change <StudentID>`.
* This will prompt the user for a password, similar to `Clean`.
* `Change` performs a query for the given ID, but then allows the user to change it.
* New records can also be added if `<StudentID>` is not already in the database.
* Records can also be deleted. This is performed by shifting each record back in the database, then deleting the final record.

## Notes
* By default, the program sleeps for 2 seconds at critical points for synchronicity testing. This can be modified or disabled in `header.h`.
