frontend: queries.o frontend.o
	g++ frontend.o queries.o -o frontend

frontend.o: frontend.cpp frontend.h queries.h ../../defines.h
	g++ -c frontend.cpp

queries.o: queries.cpp queries.h ../sockets.h ../../defines.h
	g++ -c queries.cpp

clean:
	rm *.o frontend
