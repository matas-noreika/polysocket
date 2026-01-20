# C Compiler
CC=gcc
# C Compiler Flags
CCFLAGS=-O3 -I./include/

# Rules that don't have prerequisites
.PHONY: all clean

# Directories
LOCALBIN=./bin

# Generic default rule (helps user with available make options)
all:
	@echo "available make rules:"
	@echo "build - generates all build targets"
	@echo "build-examples - generates all example build targets"
	@echo "clean - cleans all build targets"

#build-examples -> depends on the target platform
ifeq ($(OS),Windows_NT)
build-examples:
	@echo "Windows platform detected"
	$(MAKE) -j -C ./examples/platform-windows
else
build-examples:
	@echo "POSIX platform detected"
	$(MAKE) -j -C ./examples/platform-posix
endif

# General clean rule to remove all build targets
clean:
	-@rm -f $(LOCALBIN)/*.exe 2> /dev/null
	-@rm -f $(LOCALBIN)/*.elf 2> /dev/null
	@echo "clean complete."

