#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include "lexer.h"

class ParseTree
{
public:
    ParseTree(LexerToken &token);

    // get the token of the parse tree
    virtual LexerToken token() const;

    // print the tree (for debug purposes)
    virtual void print(int depth) const;

    // print the prefix for the tree
    virtual void print_prefix(int depth) const;
private:
    LexerToken _token;
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

    //advance the lexer
    virtual void next();

    // get the current token
    virtual LexerToken curtok() const;

    // non-terminal parse functions
    virtual ParseTree *parse_program();
    virtual ParseTree *parse_statement();
    virtual ParseTree *parse_expression();
    virtual ParseTree *parse_expression_prime(ParseTree *left);
    virtual ParseTree *parse_term();
    virtual ParseTree *parse_term_prime(ParseTree *left);
    virtual ParseTree *parse_factor();
    virtual ParseTree *parse_base();
    virtual ParseTree *parse_number();

private:
    Lexer &_lexer;
    LexerToken _curtok;
};
#endif
