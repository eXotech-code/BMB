frontend: queries.o frontend.o
	g++ frontend.o queries.o -o frontend

frontend.o: frontend.cpp frontend.h queries.h
	g++ -c frontend.cpp

queries.o: queries.cpp queries.h
	g++ -c queries.cpp

clean:
	rm *.o frontend
