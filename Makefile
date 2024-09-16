CC = gcc
CFLAGS = 
CPATH = .
TARGET = linux_archiver

all: clean $(TARGET)

$(TARGET): linux_archiver.o my_dirent.o
	$(CC) $^ -o $@

linux_archiver.o: linuxArchiver.c
	$(CC) $(CFLAGS) -c $^ -o $@

my_dirent.o: my_dirent.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf *.o 

rebuild: clean all
