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
 * < Statement >   ::= < Ref > < Statement' > NEWLINE
 *                     | < Var-Decl > NEWLINE
 *                     | < Print > NEWLINE
 *                     | < Record-Def >
 *                     | < Expression > NEWLINE
 */
ParseTree *Parser::parse_statement()
{
    ParseTree *result;

    if(has(IDENTIFIER)) {
        // statement which begin with an identifier
        ParseTree *var = parse_ref();
        result = parse_statement_prime(var);
    } else if(has(INTEGER_DECL) or has(REAL_DECL)) {
        result = parse_var_decl();
    } else if(has(PRINT)) {
        result = parse_print();
    } else if(has(RECORD)) {
        result = parse_record_def();
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
 *                     | < Record-Decl' >
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
    } else if(has(IDENTIFIER)) {
        return parse_record_decl_prime(left);
    } else {
        return parse_expression_prime(left);
    }
}


/*
 * < Var-Decl >    ::= < Type > < Identifier >
 *                     | < Type > LBRACKET < Bounds > RBRACKET IDENTIFIER
 *                     | < Record-Decl >
 */
ParseTree *Parser::parse_var_decl()
{
    LexerToken token = curtok();

    if(token == IDENTIFIER) {
        return parse_record_decl();
    }

    next();

    if(curtok() == LBRACKET) {
        ArrayDecl *result = new ArrayDecl(token);
        next();
        result->left(parse_bounds());
        must_be(RBRACKET);
        next();
        must_be(IDENTIFIER);
        result->right(new Var(curtok()));
        next();
        return result;
    } else {
        must_be(IDENTIFIER);
        VarDecl *result = new VarDecl(token);
        result->child(new Var(curtok()));
        next();
        return result;
    }
}


/*
 * < Record-Decl > ::= IDENTIFIER < Record-Decl' >
 */
ParseTree *Parser::parse_record_decl()
{
    must_be(IDENTIFIER);
    Var *left = new Var(curtok());
    return parse_record_decl_prime(left);
}


/*
 * < Record-Decl' >::= IDENTIFIER
 *                     | LBRACKET < Bounds > RBRACKET IDENTIFIER
 */
ParseTree *Parser::parse_record_decl_prime(ParseTree *left)
{
    //really, we just want the token to get the record type
    LexerToken token = left->token();
    delete left;

    //check for array declaration
    if(has(LBRACKET)) {
        next();
        ArrayDecl *result = new ArrayDecl(token);
        result->left(parse_bounds());
        must_be(RBRACKET);
        next();
        must_be(IDENTIFIER);
        result->right(new Var(curtok()));
        next();
        return result;
    }

    //scalar record declaration
    must_be(IDENTIFIER);
    VarDecl *result = new VarDecl(token);
    result->child(new Var(curtok()));
    next();
    return result;
}


/*
 * < Record-Def >  ::= RECORD IDENTIFIER NEWLINE < Fields > END
 */
ParseTree *Parser::parse_record_def()
{
    // probably not needed, but let's just double check
    must_be(RECORD);

    //make the result object
    next();
    must_be(IDENTIFIER);
    RecordDef *result = new RecordDef(curtok());
    next();
    must_be(NEWLINE);
    next();

    //add the fields
    parse_fields(result);

    //consume end
    must_be(END);
    next();

    return result;
}


/*
 * < Fields >      ::= < Fields > < Var-Decl > NEWLINE
 *                     | < Var-Decl > NEWLINE
 */
ParseTree *Parser::parse_fields(NaryOp *obj)
{
    // read all of the declarations
    while(has(INTEGER_DECL) or has(REAL_DECL) or has(IDENTIFIER)) {
        obj->push(parse_var_decl());
        must_be(NEWLINE);
        next();
    }

    return obj;
}


/*
 * < Bounds >      ::= < Bounds > COMMA INTLIT
 *                     | INTLIT
 */
ParseTree *Parser::parse_bounds() 
{
    bool done = false;
    ArrayIndex *result = new ArrayIndex(curtok());

    do {
        must_be(INTLIT);
        result->push(parse_number());

        //keep going so long as there is a comma
        if(has(COMMA)) {
            next();
        } else {
            done = true;
        }
    } while(not done);

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
        result = parse_ref();
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


/*
 * < Index >       ::= < Index > COMMA < Expression >
 *                     | < Expression >
 */
ParseTree *Parser::parse_index() 
{
    bool done = false;
    ArrayIndex *result = new ArrayIndex(curtok());

    do {
        result->push(parse_expression());

        //keep going so long as there is a comma
        if(has(COMMA)) {
            next();
        } else {
            done = true;
        }
    } while(not done);

    return result;
}



/*
 * < Ref >         ::= IDENTIFIER < Ref' >
 */
ParseTree *Parser::parse_ref()
{
    must_be(IDENTIFIER);
    Var *result = new Var(curtok());
    next();

    return parse_ref_prime(result);
}



/*
 *  < Ref' >        ::= DOT < Ref >
 *                      | LBRACKET < Index > RBracket
 *                      | ""
 */
ParseTree *Parser::parse_ref_prime(ParseTree *left)
{
    if(has(DOT)) {
        RecordAccess *result = new RecordAccess(curtok());
        next();
        result->left(left);
        result->right(parse_ref());
        return result;
    } else if(has(LBRACKET)) {
        ArrayAccess *result = new ArrayAccess(curtok());
        next();
        result->left(left);
        result->right(parse_index());
        must_be(RBRACKET);
        next();
        return result;
    }

    //must be the null string case.
    return left;
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
