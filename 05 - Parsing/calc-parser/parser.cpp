#include <iostream>
#include <sstream>
#include "parser.h"

//////////////////////////////////////////
// Parser Implementation
//////////////////////////////////////////

// initalize the lexer and get the first token
Parser::Parser(Lexer &_lexer) : _lexer(_lexer)
{
    // Load up the lexer's token buffer.
    next();
}


// parse the program
ParseTree *Parser::parse()
{
    return parse_program();
}


//token matches
bool Parser::has(Token tok)
{
    return _curtok == tok;
}


void Parser::must_be(Token tok)
{
    // Throw an exception if we don't match.
    if(not has(tok)) {
        // throw a parse error
        throw ParseError{_curtok};
    }
}


//advance the lexer
void Parser::next()
{
    _curtok = _lexer.next();
}


// non-terminal parse functions

/*
 * < Program >     ::= < Program > < Statement > 
 *                      | < Statement >
 */
ParseTree *Parser::parse_program()
{
    // Technically, this is not LL(1), but it is easy enough to handle 
    // this with a while loop
    while(not has(TEOF)) {
        parse_statement();
    }
    return nullptr;
}


/*
 * < Statement >   ::= < Expression > NEWLINE
 */
ParseTree *Parser::parse_statement()
{
    parse_expression();
    must_be(NEWLINE);
    next();

    return nullptr;
}


/*
 * < Expression >  ::= < Term > < Expression' >
 */
ParseTree *Parser::parse_expression()
{
    parse_term();
    parse_expression_prime();
    return nullptr;
}


/*
 * < Expression' > ::= PLUS < Term  > < Expression' >
 *                     | MINUS < Term > < Expression' >
 *                     | ""
 */
ParseTree *Parser::parse_expression_prime()
{
    if(has(PLUS)) {
        next();
        parse_term();
        parse_expression_prime();
    } else if(has(MINUS)) {
        next();
        parse_term();
        parse_expression_prime();
    }

    // nothing to do for the empty case
    return nullptr;
}


/*
 * < Term >        ::= < Factor > < Term' >
 */
ParseTree *Parser::parse_term()
{
    parse_factor();
    parse_term_prime();

    return nullptr;
}


/*
 * < Term' >       ::= TIMES  < Factor > < Term' >
 *                     | DIVIDE < Factor > < Term' >
 *                     | ""
 */
ParseTree *Parser::parse_term_prime()
{
    if(has(TIMES)) {
        next();
        parse_factor();
        parse_term_prime();
    } else if(has(DIVIDE)) {
        next();
        parse_factor();
        parse_term_prime();
    }

    //empty string, nothing to do
    return nullptr;
}


/*
 * < Factor >      ::= < Base > POW < Factor >
 *                     | < Base >
 */
ParseTree *Parser::parse_factor()
{
    parse_base();
    if(has(POW)) {
        next();
        parse_factor();
    }
    return nullptr;
}


/*
 * < Base >        ::= LPAREN < Expression > RPAREN
 *                     | MINUS < Expression > 
 *                     | < Number >
 */
ParseTree *Parser::parse_base()
{
    if(has(LPAREN)) {
        next();
        parse_expression();
        must_be(RPAREN);
        next();
    } else if(has(MINUS)) {
        next();
        parse_expression();
    } else {
        parse_number();
    }
    
    return nullptr;
}


/*
 * < Number >      ::= INTLIT
 *                     | REALLIT
 */
ParseTree *Parser::parse_number()
{
    if(has(INTLIT)) {
        next();
        return nullptr;
    } else {
        must_be(REALLIT);
        next();
    }

    return nullptr;
}


//////////////////////////////////////////
// ParseError Implementation
//////////////////////////////////////////

ParseError::ParseError(LexerToken &_tok)
{
    // capture the token
    this->_tok = _tok;

    // generate the message
    std::ostringstream os;
    os << "Unexpected Token " << _tok;

    _msg = os.str();
}


const char* ParseError::what() const _NOEXCEPT
{

    return _msg.c_str();
}


LexerToken ParseError::token() const
{
    return _tok;
}
