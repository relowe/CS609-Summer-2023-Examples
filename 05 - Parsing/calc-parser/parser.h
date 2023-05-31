#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include "lexer.h"

class ParseTree
{
    //TODO:Make this a real class.
};


class ParseError : std::exception
{
public:
    ParseError(LexerToken &tok);
    const char* what();
    LexerToken token();

private:
    LexerToken _tok;
    std::string _msg;
};


class Parser
{
public:
    Parser(Lexer &_lexer);
    ParseTree *parse();

protected:
    //token matches
    bool has(Token tok);
    void must_be(Token tok);

    //advanc the lexer
    void next();

    // non-terminal parse functions
    ParseTree *parse_program();
    ParseTree *parse_statement();
    ParseTree *parse_expression();
    ParseTree *parse_expression_prime();
    ParseTree *parse_term();
    ParseTree *parse_term_prime();
    ParseTree *parse_factor();
    ParseTree *parse_base();
    ParseTree *parse_number();

private:
    Lexer &_lexer;
    LexerToken _curtok;
};
#endif
