CC = gcc
TARGET1 = linux_archiver
TARGET2 = linux_unarchiver

all: clean $(TARGET1) $(TARGET2)

$(TARGET1): linux_archiver.o my_dirent.o archiver.o
	$(CC) $^ -o $@

$(TARGET2): linux_unarchiver.o my_dirent.o unarchiver.o
	$(CC) $^ -o $@

linux_archiver.o: linuxArchiver.c
	$(CC) -DARCH -c $^ -o $@

linux_unarchiver.o: linuxArchiver.c
	$(CC) -DUNARCH -c $^ -o $@

my_dirent.o: my_dirent.c
	$(CC) -c $^ -o $@

archiver.o: archiver.c
	$(CC) -c $^ -o $@

unarchiver.o: unarchiver.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o 
	rm -rf ../../archive.arch
	rm -rf ../test*

rebuild: clean all

clang:
	clang-format -i *.c *.h

# CC = gcc
# CFLAGS = 
# TARGET = linux_archiver

# all: clean $(TARGET)

# $(TARGET): linux_archiver.o my_dirent.o archiver.o
# 	$(CC) $^ -o $@

# linux_archiver.o: linuxArchiver.c
# 	$(CC) $(CFLAGS) -c $^ -o $@

# my_dirent.o: my_dirent.c
# 	$(CC) $(CFLAGS) -c $^ -o $@

# archiver.o: archiver.c
# 	$(CC) $(CFLAGS) -c $^ -o $@

# unarchiver.o: unarchiver.c
# 	$(CC) $(CFLAGS) -c $^ -o $@

# clean:
# 	rm -rf *.o 
# 	rm -rf ../../archive.arch
# 	rm -rf ../test*

# rebuild: clean all

# clang:
# 	clang-format -i *.c *.h


