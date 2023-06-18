#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include "lexer.h"
#include "op.h"


class ParseError : std::exception
{
public:
    ParseError(LexerToken &tok);
    virtual const char* what() const noexcept;
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
    virtual ParseTree *parse_statement_prime(ParseTree *left);
    virtual ParseTree *parse_expression();
    virtual ParseTree *parse_expression_prime(ParseTree *left);
    virtual ParseTree *parse_var_decl();
    virtual ParseTree *parse_record_decl();
    virtual ParseTree *parse_record_decl_prime(ParseTree *left);
    virtual ParseTree *parse_record_def();
    virtual ParseTree *parse_fields(NaryOp *obj);
    virtual ParseTree *parse_bounds();
    virtual ParseTree *parse_print();
    virtual ParseTree *parse_term();
    virtual ParseTree *parse_term_prime(ParseTree *left);
    virtual ParseTree *parse_factor();
    virtual ParseTree *parse_base();
    virtual ParseTree *parse_number();
    virtual ParseTree *parse_index();
    virtual ParseTree *parse_ref();
    virtual ParseTree *parse_ref_prime(ParseTree *left);

private:
    Lexer &_lexer;
    LexerToken _curtok;
};
#endif
