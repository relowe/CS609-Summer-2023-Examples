#include <iostream>
#include <random>
#include <string>

//global random number generator
auto rd = std::random_device {};
auto rng = std::default_random_engine{ rd() };

// prototypes
int select_rule(int depth, int min, int max);
std::string s(int depth);
std::string e(int depth);
std::string n(int depth);
std::string d(int depth);


//Randomly choose a rule between min and max. If remaining depth is 0, 
//force the max rule (which is assumed to be a terminal rule).
int select_rule(int depth, int min, int max)
{
    std::uniform_int_distribution rule(min,max);

    if(depth > 0) {
        return rule(rng);
    } else {
        return max;
    }
}


// generate an s rule with remaining depth
std::string s(int depth) 
{
    //rule 1
    return e(depth - 1);
}


// generate an e rule with remaining depth
std::string e(int depth) 
{
    int rule = select_rule(depth, 2,6);

    if(rule == 2) {
        return e(depth-1) + "+" + e(depth-1);
    } else if(rule == 3) {
        return e(depth-1) + "-" + e(depth-1);
    } else if(rule == 4) {
        return e(depth-1) + "*" + e(depth-1);
    } else if(rule == 5) {
        return e(depth-1) + "/" + e(depth-1);
    } 

    //rule 6
    return n(depth-1);
}


// generate an n rule with remaining depth
std::string n(int depth) 
{
    int rule = select_rule(depth, 7,8);

    if(rule == 7) {
        return n(depth-1) + d(depth-1);
    }

    //rule 8
    return d(depth-1);
}


std::string d(int depth) 
{
    //Either choice is a terminal, so force random selection.
    int rule = select_rule(1, 9, 10);
    
    if(rule == 9) {
        return "0";
    }

    //rule 10
    return "1";
}


int main()
{
    int max_depth;

    //get the desired depth
    std::cout << "Max Depth? ";
    std::cin >> max_depth;

    //generate the output
    std::cout << s(max_depth) << std::endl;
}
