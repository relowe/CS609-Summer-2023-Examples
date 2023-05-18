#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "hand.h"

Hand::Hand(const Card &down, const Card &up)
{
    _hard = 0;
    _soft = 0;
    _revealed = false;

    hit(down);
    hit(up);
}


// get / set revealed status
bool Hand::revealed() const
{
    return _revealed;
}


void Hand::revealed(bool _revealed)
{
    this->_revealed = _revealed;
}


// get best point value
int Hand::point() const
{
    if(_soft) {
        return _soft;
    }

    return _hard;
}

// get the hard value (with aces 1)
int Hand::hard() const
{
    return _hard;
}


// get soft value (zero if soft busts or no ace)
int Hand::soft() const
{
    return _soft;
}

//receive a hit
void Hand::hit(const Card &card)
{
    int value; //numeric value of the currecnt card
    std::string face = card.value();
             
    _cards.push_back(card);
   
    //get the card's value
    if(face == "A") {
        value = 1;
    } else if(face == "J" or face == "Q" or face == "K") {
        value = 10;
    } else {
        value = std::stoi(face);
    }

    //add to the hard score
    _hard += value;
   
    if(_soft or value == 1) {
        _soft = _hard + 10;
    }

    //reset a busted soft
    if(_soft>21) {
        _soft = 0;
    }
}


//const iterators for observing hand
std::vector<Card>::const_iterator Hand::begin() const
{
    return _cards.begin();
}


std::vector<Card>::const_iterator Hand::end() const
{
    return _cards.end();
}


// Insertion operator to print a hand
std::ostream& operator<<(std::ostream &os, const Hand &hand)
{
    auto itr = hand.begin();

    // print the first card
    if(hand.revealed()) {
        os << *itr << ' ';
    } else {
        os << "### ";
    }
    
    //print the rest of the cards
    for(itr++; itr != hand.end(); itr++) {
        os << *itr << ' ';
    }

    //print the score for revealed hands
    if(hand.revealed()) {
        os << "(";
        os << hand.hard();
        int soft = hand.soft();
        if(soft) {
            os << " / " << soft;
        }
        os << ")";
    }

    return os;
}
