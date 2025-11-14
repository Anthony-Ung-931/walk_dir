CC = cc
CFLAGS = -m64 -std=c89
BINS = walk_dir
CFILES = walk_dir.c

all: $(BINS)

foo.o: foo.c
	$(CC) $(CFLAGS) -o3 -c -g walk_dir.c

.PHONY: clean
clean:
	rm -f $(BINS) *.o

