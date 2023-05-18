#include <iostream>
#include <iomanip>
#include <string>
#include "card.h"

Card::Card(const std::string &value, char suit)
{
    this->_suit = suit;
    this->_value = value;
}

const std::string& Card::value() const
{
    return _value;
}

char Card::suit() const
{
    return _suit;
}


// Print out a card
std::ostream &operator<<(std::ostream &os, const Card &card) 
{
    // print out the value
    return os << std::setw(2) << card.value() << card.suit();
}
