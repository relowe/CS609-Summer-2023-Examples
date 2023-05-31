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
    virtual const char* what() const _NOEXCEPT;
    virtual LexerToken token() const;

private:
    LexerToken _tok;
    std::string _msg;
};


class Parser
{
public:
    Parser(Lexer &_lexer);
    virtual ParseTree *parse();

protected:
    //token matches
    virtual bool has(Token tok);
    virtual void must_be(Token tok);

    //advanc the lexer
    virtual void next();

    // non-terminal parse functions
    virtual ParseTree *parse_program();
    virtual ParseTree *parse_statement();
    virtual ParseTree *parse_expression();
    virtual ParseTree *parse_expression_prime();
    virtual ParseTree *parse_term();
    virtual ParseTree *parse_term_prime();
    virtual ParseTree *parse_factor();
    virtual ParseTree *parse_base();
    virtual ParseTree *parse_number();

private:
    Lexer &_lexer;
    LexerToken _curtok;
};
#endif
