// This file contains the classes which implement parse 
// trees of the calc operations. 
#ifndef OP_H
#define OP_H
#include <iostream>
#include <vector>
#include <map>
#include "lexer.h"


//////////////////////////////////////////
// Multi-Typed Result Returns
//////////////////////////////////////////
union ResultField
{
    int i;
    double r;
    void *ptr;  // Pointer to other field types
};


enum ResultType 
{
    VOID=0,
    INTEGER,
    REAL,
    RECORD_DEF,
    RECORD_VAR,
    ARRAY_VAR
};


struct Result
{
    // the value and type of the result
    ResultField val;
    ResultType type;
};

// convert result types to strings
extern const char* RTSTR[];

// print result values
std::ostream& operator<<(std::ostream& os, const Result &result);

// A macro to extract the numeric result from Result
#define NUM_RESULT(res) ((res).type == INTEGER ? (res).val.i : (res).val.r)

// A macro to assign the correct numeric field
#define NUM_ASSIGN(res, n) ((res).type == INTEGER ? (res).val.i=(n) : (res).val.r=(n))


//////////////////////////////////////////
// Variable Storage
//////////////////////////////////////////
class RefEnv {
public:
    // constructor
    RefEnv();

    // declare a variable
    virtual void declare(const std::string &name, ResultType type);

    // check to see if a name exists in the environment
    virtual bool exists(const std::string &name);

    // retrieve a variable associative array style
    virtual Result& operator[](const std::string &name);

private:
    std::map<std::string, Result> _symtab;
};


//////////////////////////////////////////
// Base Classes
//////////////////////////////////////////
class ParseTree
{
public:
    //constructor and destructor
    ParseTree(LexerToken &token);
    virtual ~ParseTree();

    // get the token of the parse tree
    virtual LexerToken token() const;

    // evaluate the parse tree
    virtual Result eval(RefEnv &env)=0;

    // print the tree (for debug purposes)
    virtual void print(int depth) const;

    // print the prefix for the tree
    virtual void print_prefix(int depth) const;
private:
    LexerToken _token;
};


// Base class for unary operations
class UnaryOp : public ParseTree
{
public:
    // constructor
    UnaryOp(LexerToken &_token);

    // destructor
    virtual ~UnaryOp();

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
    virtual ~BinaryOp();

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
    virtual ~NaryOp();

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
    virtual Result eval(RefEnv &env);
    virtual void print(int depth) const;
};


// An Add Operation
class Add : public BinaryOp
{
public:
    Add(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A Subtract Operation
class Sub : public BinaryOp
{
public:
    Sub(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A Multiply Operation
class Mul: public BinaryOp
{
public:
    Mul(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A Divide Operation
class Div: public BinaryOp
{
public:
    Div(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A Power Operation
class Pow: public BinaryOp
{
public:
    Pow(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A Negate operations
class Neg: public UnaryOp 
{
public:
    Neg(LexerToken _token);
    virtual Result eval(RefEnv &env);
    virtual void print(int depth) const;
};


// A Literal Number
class Number: public ParseTree
{
public:
    Number(LexerToken _token);
    virtual Result eval(RefEnv &env);
protected:
    Result _val;
};


// Abstract class for accessing records
class Accessor : public ParseTree
{
public:
    Accessor(LexerToken _token);
    virtual Result& eval_ref(RefEnv &env)=0;
};


// A Variable Retrieval
class Var: public Accessor
{
public:
    Var(LexerToken _token);
    virtual Result eval(RefEnv &env);
    virtual Result &eval_ref(RefEnv &env);
};


// A print operation
class Print: public UnaryOp 
{
public:
    Print(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A variable declaration operation
class VarDecl: public UnaryOp 
{
public:
    VarDecl(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// An Assignment Operation
class Assign : public BinaryOp
{
public:
    Assign(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// An array access operation
class ArrayDecl: public BinaryOp 
{
public:
    ArrayDecl(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// An array Access operation
class ArrayAccess: public BinaryOp, public Accessor
{
public:
    ArrayAccess(LexerToken _token);
    virtual Result eval(RefEnv &env);
    virtual Result &eval_ref(RefEnv &env);
};


// An array index node
class ArrayIndex: public NaryOp
{
public:
    ArrayIndex(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A record definition operation
class RecordDef: public NaryOp 
{
public:
    RecordDef(LexerToken _token);
    virtual Result eval(RefEnv &env);
};


// A record access operation
class RecordAccess: public BinaryOp, public Accessor
{
public:
    RecordAccess(LexerToken _token);
    virtual Result eval(RefEnv &env);
    virtual Result &eval_ref(RefEnv &env);
};
#endif
