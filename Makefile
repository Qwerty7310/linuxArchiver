CC = gcc
TARGET1 = linux_archiver
TARGET2 = linux_unarchiver
CFLAGS = 
CPATH = ./bin

SRC_FILES = linux_archiver.c my_dirent.c archiver.c unarchiver.c compress.c

# OBJ_FILES = $(addprefix $(CPATH)/, $(SRC_FILES:.c=.o))

all: clean $(TARGET1) $(TARGET2)

$(TARGET1): $(CPATH)/linux_archiver.o $(CPATH)/my_dirent.o $(CPATH)/archiver.o $(CPATH)/compress.o
	$(CC) $^ -o $(CPATH)/$@

$(TARGET2): $(CPATH)/linux_unarchiver.o $(CPATH)/my_dirent.o $(CPATH)/unarchiver.o $(CPATH)/compress.o
	$(CC) $^ -o $(CPATH)/$@

$(CPATH)/linux_archiver.o: linuxArchiver.c
	$(CC) $(CFLAGS) -DARCH -c $^ -o $@

$(CPATH)/linux_unarchiver.o: linuxArchiver.c
	$(CC) $(CFLAGS) -DUNARCH -c $^ -o $@

$(CPATH)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# my_dirent.o: my_dirent.c
# 	$(CC) -c $^ -o $(CPATH)/$@

# archiver.o: archiver.c
# 	$(CC) -c $^ -o $(CPATH)/$@

# unarchiver.o: unarchiver.c
# 	$(CC) -c $^ -o $(CPATH)/$@

# compreress.o: compress.c
# 	$(CC) -c $^ -o $(CPATH)/$@

clean:
	rm -rf $(CPATH)/*
	rm -rf ../../archive.arch
	rm -rf ../test*
	rm -rf *.tmp
	rm -rf *.arch
	rm -rf *.bin
	rm -rf *.out

rebuild: clean all

clang:
	clang-format -i *.c *.h
