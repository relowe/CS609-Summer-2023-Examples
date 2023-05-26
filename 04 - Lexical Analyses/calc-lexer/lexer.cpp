#include <iostream>
#include <string>
#include <functional>
#include <cctype>
#include "lexer.h"

// translate tokens into strings for easy debugging
const char* TSTR[] = {
    "INVALID",
    "EOF",
    "NEWLINE",
    "PLUS",
    "MINUS",
    "TIMES",
    "DIVIDE",
    "POW",
    "LPAREN",
    "RPAREN",
    "INTLIT",
    "REALLIT"
};


//////////////////////////////////////////
// LexerToken Functions
//////////////////////////////////////////

// equality operators
bool LexerToken::operator==(const Token &rhs) const
{
    return this->token == rhs;
}


bool LexerToken::operator==(const LexerToken &rhs) const
{
    return *this == rhs.token;
}


bool LexerToken::operator!=(const Token &rhs) const
{
    return this->token != rhs;
}


bool LexerToken::operator!=(const LexerToken &rhs) const
{
    return *this != rhs.token;
}


// print the lexter token (mainly for debugging)
std::ostream& operator<<(std::ostream &os, const LexerToken &t)
{
    return os << TSTR[t.token] << ": \"" << t.lexeme 
              << "\" Line: " << t.line
              << " Column: " << t.col;
}



//////////////////////////////////////////
// Lexer Implementation
//////////////////////////////////////////

// Construct a lexer using std::cin
Lexer::Lexer() : Lexer(std::cin)
{
    // Note the above is how we do constructor delegation!
    // this space left intentionally blank
}


// Construct a lexer for the given stream
Lexer::Lexer(std::istream &is) : _is(is)
{
    _cur = '\0';        // Start with a null character
    _line = 1;          // Humans start counting at 1
    _col = 0;           // We have not scanned a character yet
    _sigline = false;   // No significant characters found yet

    // start off with an invalid token
    _curtok.token = INVALID;
    _curtok.line = _line;
    _curtok.col = _col;
}


// advance the lexer to the next token
LexerToken Lexer::next()
{
    // get to a significant charcater
    skip();

    // mark the beginning of the current token (assume it is invalid)
    _curtok.token = INVALID;
    _curtok.lexeme = "";
    _curtok.line = _line;
    _curtok.col = _col;

    // Try each class of token
    if(not _is) {
        _curtok.token = TEOF;
        return current();
    } else if(lex_single()) {
        return current();
    } else if(lex_number()) {
        return current();
    } else {
        // nothing matched, consume and move on
        consume();
        return current();
    }
}


// get the current token
LexerToken Lexer::current() const
{
    return _curtok;
}


// read the next character from the stream
void Lexer::read()
{
    // get the character from the current stream
    _cur = _is.get();

    // update line and column information 
    if(_cur == '\n') {
        _line++;
        _col=0;
        _sigline = false;
    } else {
        _col++;
    }
}


// consume the current character and add it to the lexeme
void Lexer::consume()
{
    // add the current character to the lexeme and get the next one
    _curtok.lexeme += _cur;
    read();
}

// consume all the characters that match the comparison pattern
void Lexer::consume(std::function<bool(char)> match)
{
    while(match(_cur)) {
        consume();
    }
}


// skip irrelevant spaces and symbols
void Lexer::skip()
{
    // read until the next significant character is found
    while((_is and _cur == '\0') or
          (not _sigline and _cur =='\n') or
          isspace(_cur)) 
    {
        read();
    }

    // once we are here, we have a significant character
    _sigline = true;
}


// attempt to lex the single character tokens
bool Lexer::lex_single()
{

    // assume we have an invalid token
    _curtok.token = INVALID;

    // match our character
    switch(_cur) 
    {
        case '\n':
            _curtok.token = NEWLINE;
            break;

        case '+':
            _curtok.token = PLUS;
            break;
            
        case '-':
            _curtok.token = MINUS;
            break;

        case '*':
            _curtok.token = TIMES;
            break;

        case '/':
            _curtok.token = DIVIDE;
            break;

        case '^':
            _curtok.token = POW;
            break;

        case '(':
            _curtok.token = LPAREN;
            break;

        case ')':
            _curtok.token = RPAREN;
            break;

        default:
            return false;
    }


    if(_curtok == INVALID) {
        // no match was made
        return false;
    } else {
        // consume the match and move on
        consume();
        return true;
    }
}


// attempt to lex a number
bool Lexer::lex_number()
{
    //numbers must begin with a digit
    if(not isdigit(_cur)) {
        return false;
    }

    //we have entered the INTLIT state
    _curtok.token = INTLIT;

    // consume the digits
    consume(isdigit);

    //if there is no dot, we are done
    if(_cur != '.') {
        return true;
    }

    //ok, so we have a dot! consume it and transition to invalid
    consume();
    _curtok.token = INVALID;

    //Now, if the next symbol is not a number, we have succesfully
    //matched an invalid token. 
    if(not isdigit(_cur)) {
        return true;
    }

    // it's a real literal! consume the rest and return true
    _curtok.token = REALLIT;
    consume(isdigit);
    return true;
}


