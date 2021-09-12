
all: Change Load Query Print Clean

Change: change.c header.h
	gcc -Wall -o Change change.c header.c
Load: load.c header.h
	gcc -Wall -o Load load.c header.c
Query: query.c header.h
	gcc -Wall -o Query query.c header.c
Print: print.c header.h
	gcc -Wall -o Print print.c header.c
Clean: clean.c header.h
	gcc -Wall -o Clean clean.c header.c

clean:
	rm -f Change Load Query Print Clean clean_output.txt *~ core
