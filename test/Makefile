CC=gcc
CFLAGS=-O2 -Wall -I..

all: test_physfs example

test_physfs: test_physfs.c physfs.c physfs_platform.c ../miniphysfs.h
	$(CC) $(CFLAGS) test_physfs.c physfs_platform.c physfs.c -o test_physfs

example: example.c ../miniphysfs.h
	$(CC) $(CFLAGS) example.c -o example

clean:
	rm -f test_physfs example
