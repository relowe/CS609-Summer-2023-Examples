#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <string>
#include <functional>


//Token enumeration
enum Token
{
    INVALID=0,
    TEOF,
    NEWLINE,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    POW,
    LPAREN,
    RPAREN,
    INTLIT,
    REALLIT,
    PRINT,
    IDENTIFIER,
    INTEGER_DECL,
    REAL_DECL,
    EQUAL,
    RECORD,
    END,
    LBRACKET,
    RBRACKET,
    COMMA,
    DOT
};

// translate tokens into strings for easy debugging
extern const char* TSTR[];

// Store a detailed account of a token, including the token 
// along with its lexeme, line, and column
struct LexerToken 
{
    Token token;
    std::string lexeme;
    int line;
    int col;

    virtual bool operator==(const Token &rhs) const;
    virtual bool operator==(const LexerToken &rhs) const;
    virtual bool operator!=(const Token &rhs) const;
    virtual bool operator!=(const LexerToken &rhs) const;
};


// print the lexter token (mainly for debugging)
std::ostream& operator<<(std::ostream &os, const LexerToken &t);


class Lexer
{
public:
    // Construct a lexer using std::cin
    Lexer();

    // Construct a lexer for the given stream
    Lexer(std::istream &is);

    // advance the lexer to the next token
    virtual LexerToken next();

    // get the current token
    virtual LexerToken current() const;

protected:
    // read the next character from the stream
    virtual void read();
    
    // consume the current character and add it to the lexeme
    virtual void consume();

    // consume all the characters that match the comparison pattern
    virtual void consume(std::function<bool(char)>match);

    // skip irrelevant spaces and symbols
    virtual void skip();

    // attempt to lex the single character tokens
    virtual bool lex_single();

    // attempt to lex a number
    virtual bool lex_number();

    // attempt to lex a keyword or identifier
    virtual bool lex_kw_id();

private:
    std::istream &_is;      // The stream we are lexing
    LexerToken _curtok;     // The current token
    char _cur;              // The current character
    int _line;              // The current line we are lexing
    int _col;               // The current column we are lexing
    bool _sigline;          // True if significant characters have been found
};

#endif
