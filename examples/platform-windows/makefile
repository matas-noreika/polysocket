# Safety check to ensure user is on windows
ifneq ($(OS),Windows_NT)
$(error You are not on windows platform!)
endif

all: build-windows-examples

build-windows-examples: build-windows-client-examples build-windows-server-examples

build-windows-server-examples:
	$(MAKE) -C ./server

build-windows-client-examples:
	$(MAKE) -C ./client

clean:
	$(MAKE) -C ./server clean
	$(MAKE) -C ./client clean