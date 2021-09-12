

all: create change load query

create: create.c header.h
	gcc -Wall -o create create.c header.c
change: change.c header.h
	gcc -Wall -o change change.c header.c
load: load.c header.h
	gcc -Wall -o load load.c header.c
query: query.c header.h
	gcc -Wall -o query query.c header.c

clean:
	rm -f create change load query *~ core
