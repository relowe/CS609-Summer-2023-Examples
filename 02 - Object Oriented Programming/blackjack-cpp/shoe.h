#ifndef SHOE_H
#define SHOE_H
#include <vector>
#include "card.h"

class Shoe
{
public:
    // create a shoe of n decks
    Shoe(int n);

    Card deal();
    
    unsigned int remaining() const;
private:
    std::vector<Card> _contents;       // The contents of the shoe
    std::vector<Card>::iterator _top;  // The top card in the shoe
};
#endif
