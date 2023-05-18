#ifndef HAND_H
#define HAND_H
#include <vector>
#include <iostream>
#include "card.h"

class Hand 
{
public:
    Hand(const Card &down, const Card &up);

    // get / set revealed status
    bool revealed() const;
    void revealed(bool _revealed);

    // get best point value
    int point() const;

    // get the hard value (with aces 1)
    int hard() const;

    // get soft value (zero if soft busts or no ace)
    int soft() const;

    //receive a hit
    void hit(const Card &card);

    //const iterators for observing hand
    std::vector<Card>::const_iterator begin() const;
    std::vector<Card>::const_iterator end() const;

private:
    std::vector<Card> _cards;
    int _hard;
    int _soft;
    bool _revealed;
};


// Insertion operator to print a hand
std::ostream& operator<<(std::ostream &os, const Hand &hand);
#endif
