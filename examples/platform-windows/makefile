#Safety check to ensure user is on windows
ifneq ($(OS),Windows_NT)
$(error You are not on windows platform!)
endif

build-windows-examples: build-windows-client-examples build-windows-server-examples

build-windows-server-examples:
	$(MAKE) -j -C ./server

build-windows-client-examples:
	$(MAKE) -j -C ./client/
