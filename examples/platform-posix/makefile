ifeq ($(OS),Windows_NT)
$(error You are not on a POSIX compatible machine)
endif

# Rule to build all posix examples
build-posix-examples: build-posix-server-examples build-posix-client-examples

# Rule to call recursive makefile in server directory
build-posix-server-examples:
	$(MAKE) -j -C ./server

# Rule ro call recursive makefile in client directory
build-posix-client-examples:
	$(MAKE) -j -C ./client/
