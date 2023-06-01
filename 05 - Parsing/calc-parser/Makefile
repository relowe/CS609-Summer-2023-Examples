CXXFLAGS=-g
TARGETS= lexer_test parser_test

all: $(TARGETS)

lexer_test: lexer_test.o lexer.o
	g++ -o $@ $^ $(CXXFLAGS)

parser_test: parser_test.o lexer.o parser.o op.o
	g++ -o $@ $^ $(CXXFLAGS)

lexer_test.o: lexer.h lexer_test.cpp
	g++ -c $(CXXFLAGS) lexer_test.cpp

parser_test.o: lexer.h parser.h op.h parser_test.cpp
	g++ -c $(CXXFLAGS) parser_test.cpp

lexer.o: lexer.cpp lexer.h
	g++ -c $(CXXFLAGS) lexer.cpp

parser.o: parser.cpp parser.h
	g++ -c $(CXXFLAGS) parser.cpp

op.o: op.h op.cpp
	g++ -c $(CXXFLAGS) op.cpp

clean:
	rm -f *.o $(TARGETS)
