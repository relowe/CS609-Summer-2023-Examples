CXXFLAGS=-g --std=c++20

blackjack: blackjack.o card.o shoe.o hand.o
	g++ -o $@ $^ $(CXXFLAGS)

blackjack.o: blackjack.cpp
	g++ -c -o $@ blackjack.cpp $(CXXFLAGS)

card.o: card.cpp card.h
	g++ -c -o $@ card.cpp $(CXXFLAGS)

shoe.o: shoe.cpp shoe.h
	g++ -c -o $@ shoe.cpp $(CXXFLAGS)

hand.o: hand.cpp hand.h
	g++ -c -o $@ hand.cpp $(CXXFLAGS)

clean:
	rm -f *.o blackjack
