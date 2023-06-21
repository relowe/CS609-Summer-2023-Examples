#include <iostream>
#include <iomanip>
#include <sstream>
#include "parser.h"
#include "op.h"

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


// get the current token
LexerToken Parser::curtok() const
{
    return _curtok;
}


// non-terminal parse functions

/*
 * < Program >     ::= < Program > < Statement > 
 *                      | < Statement >
 */
ParseTree *Parser::parse_program()
{
    Program *result = new Program(curtok());

    // Technically, this is not LL(1), but it is easy enough to handle 
    // this with a while loop
    while(not has(TEOF)) {
        result->push(parse_statement());
    }
    return result;
}


/*
 * < Statement >   ::= < Identifier > < Statement' > NEWLINE
 *                     | < Var-Decl > NEWLINE
 *                     | < Print > NEWLINE
 *                     | < Expression > NEWLINE
 *                     | < While > NEWLINE
 *                     | < Branch > NEWLINE
 */
ParseTree *Parser::parse_statement()
{
    ParseTree *result;

    if(has(IDENTIFIER)) {
        // statement which begin with an identifier
        ParseTree *var = new Var(curtok());
        next();
        result = parse_statement_prime(var);
    } else if(has(INTEGER_DECL) or has(REAL_DECL)) {
        result = parse_var_decl();
    } else if(has(PRINT)) {
        result = parse_print();
    } else if(has(WHILE)) {
        result = parse_while();
    } else if(has(IF)) {
        result = parse_branch();
    } else {
        result = parse_expression();
    }

    // handle the newline at the end of the statement
    must_be(NEWLINE);
    next();

    return result;
}


/*
 * < Statement' >  ::= EQUAL < Expression > 
 *                     | < Expression' >
 */
ParseTree *Parser::parse_statement_prime(ParseTree *left)
{
    if(has(EQUAL)) {
        Assign *result;
        result = new Assign(curtok());
        next();
        result->left(left);
        result->right(parse_expression());
        return result;
    } else {
        return parse_expression_prime(left);
    }
}


/*
 * < Var-Decl >    ::= < Type > < Identifier >
 */
ParseTree *Parser::parse_var_decl()
{
    VarDecl *result = new VarDecl(curtok());
    next();
    must_be(IDENTIFIER);
    result->child(new Var(curtok()));
    next();

    return result;
}


/*
 * < Print >       ::= PRINT < Expression >
 */
ParseTree *Parser::parse_print()
{
    Print *result = new Print(curtok());
    next();
    result->child(parse_expression());
    return result;
}


/*
 * < While >       ::= WHILE < Condition > NEWLINE < Block >
 */
ParseTree *Parser::parse_while()
{
    must_be(WHILE);
    While *result = new While(curtok());
    next();
    result->left(parse_condition());
    must_be(NEWLINE);
    next();
    result->right(parse_block());
    return result;
}


/*
 * < Branch >      ::= IF < Condition > NEWLINE < Block >
 */
ParseTree *Parser::parse_branch()
{
    must_be(IF);
    Branch *result = new Branch(curtok());
    next();
    result->left(parse_condition());
    must_be(NEWLINE);
    next();
    result->right(parse_block());

    return result;
}


/*
 * < Condition>    ::= < Expression > EQUAL < Expression >
 *                     | < Expression > NOTEQUAL  < Expression >
 */
ParseTree *Parser::parse_condition() 
{
    ParseTree *lexpr = parse_expression();
    BinaryOp *result;
    if(has(EQUAL)) {
        result = new Equal(curtok());
        next();
    } else {
        must_be(NOTEQUAL);
        result = new NotEqual(curtok());
        next();
    }
    result->left(lexpr);
    result->right(parse_expression());

    return result;
}


/*
 * < Block >       ::= < Block > < Statement > 
 *                     | < Statement > END
 */
ParseTree *Parser::parse_block()
{
    Program *result = new Program(curtok());

    do {
        result->push(parse_statement());
    } while(not has(END));

    next();
    return result;
}


/*
 * < Expression >  ::= < Term > < Expression' >
 */
ParseTree *Parser::parse_expression()
{
    ParseTree *left = parse_term();
    return parse_expression_prime(left);
}


/*
 * < Expression' > ::= PLUS < Term  > < Expression' >
 *                     | MINUS < Term > < Expression' >
 *                     | ""
 */
ParseTree *Parser::parse_expression_prime(ParseTree *left)
{
    if(has(PLUS)) {
        // start the parse tree
        Add *result = new Add(curtok());
        next();

        //get the children
        result->left(left);
        result->right(parse_term());
        return parse_expression_prime(result);
    } else if(has(MINUS)) {
        // start the parse tree
        Sub *result = new Sub(curtok());
        next();

        // get the children
        result->left(left);
        result->right(parse_term());
        return parse_expression_prime(result);
    }

    // nothing to do for the empty case
    return left;
}


/*
 * < Term >        ::= < Factor > < Term' >
 */
ParseTree *Parser::parse_term()
{
    ParseTree *left = parse_factor();
    return parse_term_prime(left);
}


/*
 * < Term' >       ::= TIMES  < Factor > < Term' >
 *                     | DIVIDE < Factor > < Term' >
 *                     | ""
 */
ParseTree *Parser::parse_term_prime(ParseTree *left)
{
    if(has(TIMES)) {
        // start the parse tree
        Mul *result = new Mul(curtok());
        next();

        // get the children
        result->left(left);
        result->right(parse_factor());
        return parse_term_prime(result);
    } else if(has(DIVIDE)) {
        // start the parse tree
        Div *result = new Div(curtok());
        next();

        result->left(left);
        result->right(parse_factor());
        return parse_term_prime(result);
    }

    //empty string, nothing to do
    return left;
}


/*
 * < Factor >      ::= < Base > POW < Factor >
 *                     | < Base >
 */
ParseTree *Parser::parse_factor()
{
    ParseTree *left = parse_base();
    if(has(POW)) {
        //create the parse tree
        Pow *result = new Pow(curtok());
        next();

        // get the children
        result->left(left);
        result->right(parse_factor());
        return result;
    }
    return left;
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
        ParseTree *result = parse_expression();
        must_be(RPAREN);
        next();
        return result;
    } else if(has(MINUS)) {
        Neg *result = new Neg(curtok());
        next();
        result->child(parse_expression());
        return result;
    } else {
        return parse_number();
    }
}


/*
 * < Number >      ::= INTLIT
 *                     | REALLIT
 *                     | IDENTIFIER
 */
ParseTree *Parser::parse_number()
{
    ParseTree *result;

    if(has(IDENTIFIER)) {
        result = new Var(curtok());
        next();
    } else if(has(INTLIT)) {
        result = new Number(curtok());
        next();
    } else {
        must_be(REALLIT);
        result = new Number(curtok());
        next();
    }

    return result;
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


const char* ParseError::what() const noexcept
{

    return _msg.c_str();
}


LexerToken ParseError::token() const
{
    return _tok;
}
