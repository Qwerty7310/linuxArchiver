CC = gcc
CFLAGS = 
TARGET = linux_archiver

all: clean $(TARGET)

$(TARGET): linux_archiver.o my_dirent.o archiver.o unarchiver.o
	$(CC) $^ -o $@

linux_archiver.o: linuxArchiver.c
	$(CC) $(CFLAGS) -c $^ -o $@

my_dirent.o: my_dirent.c
	$(CC) $(CFLAGS) -c $^ -o $@

archiver.o: archiver.c
	$(CC) $(CFLAGS) -c $^ -o $@

unarchiver.o: unarchiver.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf *.o 
	rm -rf ../../archive.arch
	rm -rf ../test1

rebuild: clean all

clang:
	clang-format -i *.c *.h

