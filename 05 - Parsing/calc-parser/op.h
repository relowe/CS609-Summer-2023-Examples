// This file contains the classes which implement parse 
// trees of the calc operations. 
#ifndef OP_H
#define OP_H
#include <vector>
#include "parser.h"

//////////////////////////////////////////
// Base Classes
//////////////////////////////////////////

// Base class for unary operations
class UnaryOp : public ParseTree
{
public:
    // constructor
    UnaryOp(LexerToken &_token);

    // destructor
    ~UnaryOp();

    // give access to the child
    virtual ParseTree *child() const;
    virtual void child(ParseTree *_child);

    // print the tree with 1 child
    virtual void print(int depth) const;
protected:
    ParseTree *_child;    
};


// Base class for a binary operation
class BinaryOp : public ParseTree
{
public:
    //constructors
    BinaryOp(LexerToken &_token);

    //destructor
    ~BinaryOp();

    // give access to the left child
    virtual ParseTree *left() const;
    virtual void left(ParseTree *child);

    // give access to the right child
    virtual ParseTree *right() const;
    virtual void right(ParseTree *child);

    // print the tree with 2 children
    virtual void print(int depth) const;

protected:
    ParseTree *_lchild;    
    ParseTree *_rchild;    
};


// Base class for n-arry operators (lists of children)
class NaryOp : public ParseTree
{
public:
    // constructor and destructor
    NaryOp(LexerToken _token);
    ~NaryOp();

    // push a child onto the list
    virtual void push(ParseTree *child);

    // access iterators for the children
    virtual std::vector<ParseTree*>::const_iterator begin() const;
    virtual std::vector<ParseTree*>::const_iterator end() const;

    // print the tree
    virtual void print(int depth) const;
protected:
    std::vector<ParseTree*> _children;
};


//////////////////////////////////////////
// CalcOperations
//////////////////////////////////////////

// A calc program
class Program : public NaryOp
{
public:
    Program(LexerToken _token);
    virtual void print(int depth) const;
};


// An Add Operation
class Add : public BinaryOp
{
public:
    Add(LexerToken _token);
};


// A Subtract Operation
class Sub : public BinaryOp
{
public:
    Sub(LexerToken _token);
};


// A Multiply Operation
class Mul: public BinaryOp
{
public:
    Mul(LexerToken _token);
};


// A Divide Operation
class Div: public BinaryOp
{
public:
    Div(LexerToken _token);
};


// A Power Operation
class Pow: public BinaryOp
{
public:
    Pow(LexerToken _token);
};


// A Negate operations
class Neg: public UnaryOp 
{
public:
    Neg(LexerToken _token);

    virtual void print(int depth) const;
};


// A Literal Number
class Number: public ParseTree
{
public:
    Number(LexerToken _token);
};
#endif
