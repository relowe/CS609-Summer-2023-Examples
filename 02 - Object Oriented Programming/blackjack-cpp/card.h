#ifndef CARD_H
#define CARD_H
#include <string>
#include <iostream>

class Card 
{
public: 
    Card(const std::string &value, char suit);
    const std::string& value() const;
    char suit() const;

private:
    char _suit;
    std::string _value;
};

std::ostream &operator<<(std::ostream &os, const Card &card);

#endif
