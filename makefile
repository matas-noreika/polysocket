# C Compiler
CC=gcc
# C Compiler Flags
CCFLAGS=-O3 -I./include/
# Rules that don't have prerequisites
.PHONY: all clean

# Generic default rule (helps user with available make options)
all:
	@echo "available make rules:"
	@echo "build - generates all build targets"
	@echo "clean - cleans all build targets"

# General clean rule to remove all build targets
clean:
	-rm -f *.exe 2> /dev/null
	-rm -f *.o 2> /dev/null
	-rm -f *.d 2> /dev/null
	@echo "clean complete."

