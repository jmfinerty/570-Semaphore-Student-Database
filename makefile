
all: create change load query print cln

create: create.c header.h
	gcc -Wall -o create create.c header.c
change: change.c header.h
	gcc -Wall -o change change.c header.c
load: load.c header.h
	gcc -Wall -o load load.c header.c
query: query.c header.h
	gcc -Wall -o query query.c header.c
print: print.c header.h
	gcc -Wall -o print print.c header.c
cln: clean.c header.h
	gcc -Wall -o clean clean.c header.c

clean:
	rm -f create change load query print clean clean_output.txt *~ core
