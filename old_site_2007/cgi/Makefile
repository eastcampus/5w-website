CC=gcc
CFLAGS= -Wall -O2
LIBS=

all:	tsvdb.o directory.o vector.o CGIparse.o
	$(CC) $(CFLAGS) $(LIBS) -o directory.cgi *.o
	chmod 755 directory.cgi

*.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o directory.cgi
