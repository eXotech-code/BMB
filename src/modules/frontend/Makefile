frontend: queries.o frontend.o
	$(CXX) $(CXXFLAGS) frontend.o queries.o -o frontend

frontend.o: frontend.cpp frontend.h queries.h ../../defines.h
	$(CXX) $(CXXFLAGS) -c frontend.cpp

queries.o: queries.cpp queries.h ../sockets.h ../../defines.h
	$(CXX) $(CXXFLAGS) -c queries.cpp

clean:
	rm *.o frontend
