#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include "card.h"
#include "shoe.h"
#include "hand.h"

// function prototypes
void play_hand(double &money, Shoe &shoe);
void dealer_hand(Hand &hand, Shoe &shoe);
double get_bet(double money);
bool get_hit();
bool get_continue();


int main()
{
    double money = 100.00; //start with $100.00
    Shoe shoe{4};

    std::cout << "Welcome to Blackjack!" << std::endl
              << "   Dealer hits on soft 17." << std::endl
              << "Please play responsibly!" << std::endl;
    do { 
        play_hand(money, shoe);
        std::cout << "You have $" << money << std::endl;
    } while(money > 5.0 and get_continue());

    std::cout << std::endl;   // a blank line to space things out

    //print consoluation
    if(money < 5) {
        std::cout << "Awww.... better luck next time!" << std::endl;
    }

    std::cout << "Thank you for playing!" << std::endl;
}


// play a hand of blackjack given money dollars
void play_hand(double &money, Shoe &shoe) 
{
    double bet = get_bet(money);
    Hand player{shoe.deal(), shoe.deal()}, 
         dealer{shoe.deal(), shoe.deal()};
    bool done = false;

    // The player can see their own cards
    player.revealed(true);

    // Let the player build their hand.
    while(not done) {
        std::cout << "Dealer: " << dealer << std::endl;
        std::cout << "You: " << player << std::endl;
        if(player.point() >= 21) {
            done = true;
        } else if(get_hit()) {
            player.hit(shoe.deal());
        } else {
            done = true;
        }
    } 

    //reveal the dealer
    dealer.revealed(true);
    std::cout << "Dealer: " << dealer << std::endl;

    // Check for bust
    if(player.point() > 21) {
        std::cout << "You Bust" << std::endl;
        money -= bet;
    } else {
        // Let the dealer build their hand
        dealer_hand(dealer, shoe);

        if (dealer.point() > 21) {
            std::cout << "Dealer Busts! You win!" << std::endl;
            money += bet;
        } else if(dealer.point() < player.point()) {
            if(player.point() == 21) { 
                std::cout << "You have Blackjack!" << std::endl;
            }
            std::cout << "You Win!" << std::endl;
            money += bet;
        } else if(dealer.point() == player.point()) {
            std::cout << "Push" << std::endl;
        } else {
            std:: cout << "You Lose." << std::endl;
            money -= bet;
        }
    }

}


void dealer_hand(Hand &hand, Shoe &shoe)
{
    std::cout << std::endl;  // blank line to space things out a little

    while(hand.point() < 17 or hand.soft() == 17) {
        std::cout << "Dealer Hits" << std::endl;
        hand.hit(shoe.deal());
        std::cout << "Dealer: " << hand << std::endl;
    }

    if(hand.point() < 21) {
        std::cout << "Dealer Stands" << std::endl;
    } else if(hand.point() == 21) {
        std::cout << "Dealer Has Blackjack!" << std::endl;
    }
}


double get_bet(double money)
{
    double bet;
    bool done = false;

    std::cout << std::endl;  // blank line to space things out a little

    do {
        std::cout << "You have $" << money << " Enter bet (Minimum $5.00): ";
        //skip the non numeric 
        while(!isdigit(std::cin.peek())) std::cin.get();
        std::cin >> bet;

        //validate input
        if(bet < 5 or bet > money) {
            std::cout << "Invalid Bet, Please try again." << std::endl;
        } else {
            done = true;
        }
    } while(not done);

    return bet;
}


bool get_hit()
{
    bool done = false;
    char c;

    std::cout << std::endl;  // blank line to space things out a little

    do {
        //get user's choice
        std::cout << "Do you want to (S)tand or (H)it? ";
        std::cin >> c;
        
        //make it case insensitive
        c = toupper(c);

        //validate input
        if(c != 'S' and c != 'H') {
            std::cout << "Invalid Selection, please try again." << std::endl;
        } else {
            done = true;
        }
    } while(not done);

    return c == 'H';
}


bool get_continue()
{
    bool done = false;
    char c;

    std::cout << std::endl;  // blank line to space things out a little

    do {
        //get user's choice
        std::cout << "Do you want to play again (Y/N)? ";
        std::cin  >> c;

        //make it case insensitive
        c = toupper(c);

        //validate input
        if(c != 'Y' and c != 'N') {
            std::cout << "Invalid Selection, please try again." << std::endl;
        } else {
            done = true;
        }
    } while(not done);

    return c == 'Y';
}
