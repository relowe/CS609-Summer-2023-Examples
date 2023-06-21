// A small test for the lexer program
#include <iostream>
#include <fstream>
#include "lexer.h"


int main(int argc, char **argv) {
    // check the command line
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }

    // attempt to open the file
    std::ifstream file;
    file.open(argv[1]);
    if(not file) {
        std::cerr << "Error: Could not open " << argv[1] << std::endl;
        return -1;
    }

    // build the lexer and let it do its stuff.
    Lexer lexer(file);
    while(lexer.current() != TEOF) {
        std::cout << lexer.next() << std::endl;
    }

    file.close();

    return 0;
}
