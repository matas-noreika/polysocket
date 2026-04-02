CC=gcc
CFLAGS=-O3
# Link with the Windows Sockets library
LDLIBS=-lws2_32

# Retrieve all source files
SRCS=$(wildcard *.c)
BINS=$(SRCS:%.c=%.exe)
# Set the local bin directory to the parent directory's bin folder
LOCALBIN=.

all: $(BINS)
	@echo building all binaries into $(LOCALBIN)
	@echo $(BINS) $(SRCS)

# Pattern rule to convert each source file into a binary
%.exe: %.c
	$(CC) $(CFLAGS) $< -o $(LOCALBIN)/$@ $(LDLIBS)

clean:
	rm -f $(LOCALBIN)/*.exe