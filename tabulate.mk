.POSIX:

dist/table: src/tabulate.c
	$(CC) -O2 -Wall -o $@ src/tabulate.c -lcoulombho2d
