CC = gcc
CFLAGS = -Wall -Werror -Wextra
CPATH = .
TARGET = linux_archiver

all: clean $(TARGET)

$(TARGET): linux_archiver.o my_dirent.o
	$(CC) $^ -o $@

linux_archiver.o: linuxArchiver.c
	$(CC) -c $^ -o $@

my_dirent.o: my_dirent.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o 

rebuild: clean all
