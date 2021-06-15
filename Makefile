CXXFLAGS = $(DEBUG) -Wall -Wextra

.PHONY: debug clean all

all:
	$(MAKE) CXXFLAGS=$(CXXFLAGS) -C src/modules/frontend frontend
	$(MAKE) CXXFLAGS=$(CXXFLAGS) -C src/modules/main main

# Debug target appends "-g" flag to compiler
# options, so that debugging symbols are added
# at compile time.
debug:
	$(MAKE)$(MAKEFILE) clean
	$(MAKE)$(MAKEFILE) DEBUG="-g" all

clean:
	$(MAKE) -C src/modules/frontend clean
	$(MAKE) -C src/modules/main clean
