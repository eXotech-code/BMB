.PHONY: debug clean all

all:
	$(MAKE) -C src/modules/frontend frontend
	$(MAKE) -C src/modules/main main

# Debug sets flags for compiler before compiling
# all build targets
debug: CXXFLAGS += -g
debug: all

clean:
	$(MAKE) -C src/modules/frontend clean
	$(MAKE) -C src/modules/main clean
