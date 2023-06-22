// This file contains the main function and interface for the calc interpreter.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "op.h"

// Functions for the two modes of operation
static void calc_file(const char *fname);
static void calc_repl();


int main(int argc, char **argv) {
    //run the appropriate mode
    if(argc == 1) {
        calc_repl();
    } else if(argc == 2) {
        calc_file(argv[1]);
    } else {
        std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
    }
}


static void calc_file(const char *fname) 
{
    // attempt to open the file
    std::ifstream file;
    file.open(fname);

    if(!file) {
        std::cerr << "Could not open " << fname << std::endl;
        return;
    }

    try {
        // parse the program
        Lexer lex{file};
        Parser parser{lex};
        ParseTree *program = parser.parse();

        // run the program
        program->eval();

        file.close();
    } catch(ParseError e) {
        std::cerr << e.what() << std::endl;
        file.close();
    } 

}



// Read
// Eval
// Print
// Loop
static void calc_repl() 
{
    std::string line;
    bool print_tree;

    std::cout << "Print parse tree (y/n)? ";
    std::getline(std::cin, line);

    std::cout << "Type quit to exit." << std::endl;
    
    print_tree = line == "y";

    do {
        // get the line
        std::cout << "calc> ";
        std::getline(std::cin, line);
        if(!std::cin) continue;

        if(line == "quit") continue;

        // build the stream and lexer
        std::istringstream is{line+"\n"};
        Lexer lex{is};
        Parser parser{lex};

        try {
            ParseTree *program = parser.parse();
            if(print_tree) {
                program->print(0);
            }
            program->eval();
            delete program;
        } catch(ParseError e) {
            std::cerr << e.what() << std::endl;
        }
    
        // attempt to parse and run the stream
    } while(std::cin and line != "quit");
}
