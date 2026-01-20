CC=gcc
CFLAGS=-O3

# Retrieve all source files
SRCS=$(wildcard *.c)
BINS=$(SRCS:%.c=%.elf)

LOCALBIN=../../../bin

all: $(BINS)
	@echo "building all binaries into $(LOCALBIN)"
	@echo $(BINS) $(SRCS)

# Pattern rule to convert sources to binaries
$(BINS): $(SRCS)
	$(CC) $(CFLAGS) $< -o $(LOCALBIN)/$(@F)
