#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "card.h"
#include "shoe.h"

static const std::vector<char> card_suits {'S', 'C', 'D', 'H'};
static const std::vector<const char*> card_faces{"A", "2", "3", "4", 
                                                 "5", "6", "7", "8", "9", 
                                                 "10", "J", "Q", "K"};

// create a shoe of n decks
Shoe::Shoe(int n) 
{
    // put the decks in in order
    for(int i=0; i<n; i++) {
        for(auto face = card_faces.begin(); face != card_faces.end(); face++) {
            for(auto suit = card_suits.begin(); suit != card_suits.end(); suit++) {
                _contents.push_back(Card(*face, *suit));
            }
        }
    }

    //shuffle them
    auto rd = std::random_device {};
    auto rng = std::default_random_engine{ rd() };
    std::shuffle(_contents.begin(), _contents.end(),  rng);

    // point to the top card
    _top = _contents.begin();
}


Card Shoe::deal() 
{
    return *_top++;
}


unsigned int Shoe::remaining() const
{
    return _contents.end() - _top;
}
