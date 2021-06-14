all:
	$(MAKE) -C src/modules/frontend frontend
	$(MAKE) -C src/modules/main main

clean:
	$(MAKE) -C src/modules/frontend clean
	$(MAKE) -C src/modules/main clean
