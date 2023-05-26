CXXFLAGS=-g
TARGETS= lexer_test

all: $(TARGETS)

lexer_test: lexer_test.o lexer.o
	g++ -o $@ $^ $(CXXFLAGS)

lexer_test.o: lexer.h lexer_test.cpp
	g++ -c $(CXXFLAGS) lexer_test.cpp

lexer.o: lexer.cpp lexer.h
	g++ -c $(CXXFLAGS) lexer.cpp

clean:
	rm -f *.o $(TARGETS)
