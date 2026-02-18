CC=gcc
CFLAGS=-O3

# Retrieve all source files
SRCS=$(wildcard *.c)
BINS=$(SRCS:%.c=%)

LOCALBIN=../../../bin

all: $(BINS)
	@echo "building all binaries into $(LOCALBIN)"
	@echo $(BINS) $(SRCS)

# Pattern rule to convert sources to binaries
%: %.c
	$(CC) $(CFLAGS) $< -o $(LOCALBIN)/$(@F)
