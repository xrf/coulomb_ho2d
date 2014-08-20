.POSIX:

dist/table: table.c
	$(CC) -O2 -Wall -o $@ table.c -lcoulombho2d
