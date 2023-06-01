// A small test for the lexer program
#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"


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

    // build the parser and parse the file.
    try {
        Lexer lexer(file);
        Parser parser(lexer);
        ParseTree *tree = parser.parse();
        file.close();

        tree->print(0);
    } catch(ParseError e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
