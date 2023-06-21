#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include "lexer.h"
#include "op.h"


//////////////////////////////////////////
// Helper Functions and Types
//////////////////////////////////////////
static ResultType coerce(Result left, Result right) 
{
    // if the types match, there is no coercion
    if(left.type == right.type) return left.type;

    // if either left or right is void, so is the result
    if(left.type == VOID or right.type == VOID) return VOID;

    // perform type widening
    if((left.type == REAL and right.type == INTEGER) or 
       (left.type == INTEGER and right.type == REAL)) {
        return REAL;
    }

    //TODO: Technically, if we make it here we have an error. For now, we will
    //      just default to void. Eventually, we should report a type error.
    return VOID;
}


static ResultType token_to_type(Token tok)
{
    if(tok == INTEGER_DECL) {
        return INTEGER;
    } else if(tok == REAL_DECL) {
        return REAL;
    } else {
        return VOID;
    }
}


static struct ArrayVar 
{
    ArrayVar(ResultType type, const std::vector<int>& bounds) : bounds(bounds)
    {
        //set up the default entry
        Result entry;
        entry.type = type;

        //compute the number of elements
        int nelements=1;
        for(auto itr = bounds.begin(); itr != bounds.end(); itr++) {
            nelements *= *itr;
        }

        //build the array
        data.push_back(entry);
    }

    std::vector<int> bounds;
    std::vector<Result> data;
};



//////////////////////////////////////////
// Multi-Typed Result Returns
//////////////////////////////////////////

// handy string conversion for debugging
const char* RTSTR[] = { "VOID", "INTEGER", "REAL" };

// print result values
std::ostream& operator<<(std::ostream& os, const Result &result)
{
    // handle the numeric types
    if(result.type == INTEGER) return os << result.val.i;

    switch(result.type) {
        case VOID:
            break;
        case INTEGER:
            os << result.val.i;
            break;
        case REAL:
            os << result.val.r;
            break;
    }

    return os;
}


//////////////////////////////////////////
// Variable Storage
//////////////////////////////////////////

// constructor
RefEnv::RefEnv()
{
    // nothing to do
}


// declare a variable
void RefEnv::declare(const std::string &name, ResultType type)
{
    // names must be unique
    if(exists(name)) {
        throw std::runtime_error("Redeclaration of " + name);
    }

    // create the variable and add it to the table
    Result var;
    var.type = type;
    _symtab[name] = var;
}


// check to see if a name exists in the environment
bool RefEnv::exists(const std::string &name)
{
    return _symtab.find(name) != _symtab.end();
}


// retrieve a variable associative array style
Result& RefEnv::operator[](const std::string &name)
{
    // names must exist
    if(not exists(name)) {
        throw std::runtime_error(name + " not defined.");
    }

    return _symtab[name];
}



//////////////////////////////////////////
// UnaryOp Implementation
//////////////////////////////////////////

// constructor
UnaryOp::UnaryOp(LexerToken &_token) : ParseTree(_token)
{
    this->_child = nullptr;
}


// destructor
UnaryOp::~UnaryOp() 
{
    if(_child) {
        delete _child;  
    }
}


// give access to the child
ParseTree *UnaryOp::child() const
{
    return _child;
}


void UnaryOp::child(ParseTree *_child)
{
    this->_child = _child;
}


// print the tree with 1 child
void UnaryOp::print(int depth) const
{
    // print ourself
    ParseTree::print(depth);

    // print our child
    ParseTree *_child = child();
    if(_child) {
        _child->print(depth+1);
    }
}


//////////////////////////////////////////
// BinaryOp Implementation
//////////////////////////////////////////

//constructors
BinaryOp::BinaryOp(LexerToken &_token) : ParseTree(_token)
{
    _lchild = nullptr;
    _rchild = nullptr;
}


//destructor
BinaryOp::~BinaryOp()
{
    if(_lchild) {
        delete _lchild;
    }

    if(_rchild) {
        delete _rchild;
    }
}


// give access to the left child
ParseTree *BinaryOp::left() const
{
    return _lchild; 
}


void BinaryOp::left(ParseTree *child)
{
    this->_lchild = child; 
}


// give access to the right child
ParseTree *BinaryOp::right() const
{
    return _rchild;
}


void BinaryOp::right(ParseTree *child)
{
    this->_rchild = child;
}


// print the tree with 2 children
void BinaryOp::print(int depth) const
{
    ParseTree *l = left();
    ParseTree *r = right();

    // print the right child
    if(r) {
        r->print(depth+1);
    }

    // print ourself
    ParseTree::print(depth);

    // print the left child
    if(l) {
        l->print(depth+1);
    }
}


//////////////////////////////////////////
// NaryOp Implementation
//////////////////////////////////////////

// construuctor and destructor
NaryOp::NaryOp(LexerToken _token) : ParseTree(_token)
{
    // this space left intentionally blank
}


NaryOp::~NaryOp()
{
    // delete all the children
    for(auto itr = _children.begin(); itr != _children.end(); itr++) {
        delete *itr;
    }
}


// push a child onto the list
void NaryOp::push(ParseTree *child)
{
    _children.push_back(child);
}


// access iterators for the children
std::vector<ParseTree*>::const_iterator NaryOp::begin() const
{
    return _children.begin();
}


std::vector<ParseTree*>::const_iterator NaryOp::end() const
{
    return _children.end();
}


// print the tree
void NaryOp::print(int depth) const
{
    int n = _children.size();
    int m = n/2;

    // print the right hand side
    for(int i=n-1; i>=m; i--) {
        _children[i]->print(depth+1);
    }

    //print ourself
    ParseTree::print(depth);

    // print the left hand side
    for(int i=m-1; i>=0; i--) {
        _children[i]->print(depth+1);
    }
}


//////////////////////////////////////////
// Program implementation
//////////////////////////////////////////

Program::Program(LexerToken _token) : NaryOp(_token)
{
    // This space left intentionally blank
}



Result Program::eval(RefEnv &env)
{
    // evaluate each statement in the program
    for(auto itr = begin(); itr != end(); itr++) {
        (*itr)->eval(env);
    }

    // programs return void
    Result result;
    result.type = VOID;

    return result;
}


void Program::print(int depth) const
{
    int n = _children.size();
    int m = n/2;

    // print the right hand side
    for(int i=n-1; i>=m; i--) {
        _children[i]->print(depth+1);
    }

    //print ourself
    print_prefix(depth);
    std::cout << "PROGRAM" << std::endl;

    // print the left hand side
    for(int i=m-1; i>=0; i--) {
        _children[i]->print(depth+1);
    }
}


//////////////////////////////////////////
// Add implementation
//////////////////////////////////////////

Add::Add(LexerToken _token) : BinaryOp(_token)
{
    // This space left intentionally blank
}


Result Add::eval(RefEnv &env) 
{
    // evaluate the children
    Result l = left()->eval(env);
    Result r = right()->eval(env);

    // get the type of the result
    Result result;
    result.type = coerce(l, r);

    // perform the operation
    NUM_ASSIGN(result, NUM_RESULT(l) + NUM_RESULT(r));

    return result;
}


//////////////////////////////////////////
// Sub implementation
//////////////////////////////////////////

Sub::Sub(LexerToken _token) : BinaryOp(_token)
{
    // This space left intentionally blank
}


Result Sub::eval(RefEnv &env) 
{
    // evaluate the children
    Result l = left()->eval(env);
    Result r = right()->eval(env);

    // get the type of the result
    Result result;
    result.type = coerce(l, r);

    // perform the operation
    NUM_ASSIGN(result, NUM_RESULT(l) - NUM_RESULT(r));

    return result;
}


//////////////////////////////////////////
// Mul implementation
//////////////////////////////////////////

Mul::Mul(LexerToken _token) : BinaryOp(_token)
{
    // This space left intentionally blank
}


Result Mul::eval(RefEnv &env) 
{
    // evaluate the children
    Result l = left()->eval(env);
    Result r = right()->eval(env);

    // get the type of the result
    Result result;
    result.type = coerce(l, r);

    // perform the operation
    NUM_ASSIGN(result, NUM_RESULT(l) * NUM_RESULT(r));

    return result;
}


//////////////////////////////////////////
// Div implementation
//////////////////////////////////////////

Div::Div(LexerToken _token) : BinaryOp(_token)
{
    // This space left intentionally blank
}


Result Div::eval(RefEnv &env) 
{
    // evaluate the children
    Result l = left()->eval(env);
    Result r = right()->eval(env);

    // get the type of the result
    Result result;
    result.type = coerce(l, r);

    // perform the operation
    NUM_ASSIGN(result, NUM_RESULT(l) / NUM_RESULT(r));

    return result;
}


//////////////////////////////////////////
// Pow implementation
//////////////////////////////////////////

Pow::Pow(LexerToken _token) : BinaryOp(_token)
{
    // This space left intentionally blank
}


Result Pow::eval(RefEnv &env) 
{
    // evaluate the children
    Result l = left()->eval(env);
    Result r = right()->eval(env);

    // get the type of the result
    Result result;
    result.type = coerce(l, r);

    // perform the operation
    NUM_ASSIGN(result, pow(NUM_RESULT(l), NUM_RESULT(r)));

    return result;
}


//////////////////////////////////////////
// Neg implementation
//////////////////////////////////////////

Neg::Neg(LexerToken _token) : UnaryOp(_token)
{
    // This space left intentionally blank
}


Result Neg::eval(RefEnv &env)
{
    //eval the child and then negate it
    Result result = child()->eval(env);
    NUM_ASSIGN(result, -NUM_RESULT(result));

    return result;
}


void Neg::print(int depth) const
{
    print_prefix(depth);
    std::cout << "NEG: -" << std::endl;
    child()->print(depth+1);
}


//////////////////////////////////////////
// Number implementation
//////////////////////////////////////////

Number::Number(LexerToken _token) : ParseTree(_token)
{
    //get the number's value
    if(_token == INTLIT) {
        _val.type = INTEGER;
        _val.val.i = stoi(_token.lexeme);
    } else if(_token == REALLIT) {
        _val.type = REAL;
        _val.val.r = stod(_token.lexeme);
    }
}


Result Number::eval(RefEnv &env)
{
    return _val;
}


//////////////////////////////////////////
// ParseTree Implementation
//////////////////////////////////////////

ParseTree::ParseTree(LexerToken &token)
{
    this->_token = token;
}


ParseTree::~ParseTree()
{
    //nothing to do
}


// get the token of the parse tree
LexerToken ParseTree::token() const
{
    return _token;
}


// print the tree (for debug purposes)
void ParseTree::print(int depth) const
{
    print_prefix(depth);
    std::cout << TSTR[token().token] 
              << ": " << token().lexeme << std::endl;
}


// print the prefix for the tree
void ParseTree::print_prefix(int depth) const
{
    // no prefix for the root.
    if(depth == 0) return;

    for(int i=1; i<depth; i++) {
        std::cout << "  |";
    }

    if(depth > 1) {
        std::cout << "--+";
    } else {
        std::cout << "  +";
    }
}


//////////////////////////////////////////
// Accessor Implementation
//////////////////////////////////////////
Accessor::Accessor(LexerToken _token) : ParseTree(_token)
{
}


//////////////////////////////////////////
// Var Implementation
//////////////////////////////////////////
Var::Var(LexerToken _token) : Accessor(_token)
{
}


Result Var::eval(RefEnv &env)
{
    return eval_ref(env);
}


Result& Var::eval_ref(RefEnv &env)
{
    return env[token().lexeme];
}


//////////////////////////////////////////
// Print Implementation
//////////////////////////////////////////
Print::Print(LexerToken _token) : UnaryOp(_token)
{
}


Result Print::eval(RefEnv &env)
{
    Result result;
    result.type = VOID;

    //print the result of the child
    std::cout << child()->eval(env) << std::endl;

    return result;
}


//////////////////////////////////////////
// VarDecl Implementation
//////////////////////////////////////////
VarDecl::VarDecl(LexerToken _token) : UnaryOp(_token)
{
}


Result VarDecl::eval(RefEnv &env)
{
    ResultType var_type;
    Result result;
    result.type = VOID;

    //get the variable type
    var_type = token_to_type(token().token);

    //perform the declaration
    env.declare(child()->token().lexeme, var_type);

    return result;
}


//////////////////////////////////////////
// Assign Impelementation
//////////////////////////////////////////
Assign::Assign(LexerToken _token) : BinaryOp(_token)
{
}


Result Assign::eval(RefEnv &env)
{

    // get the value and name to assign
    Result val = right()->eval(env);
    std::string name = left()->token().lexeme;

    //perform the assignment
    Accessor *var = (Accessor*) left();
    Result &var_ref = var->eval_ref(env);
    NUM_ASSIGN(var_ref, NUM_RESULT(val));

    Result result;
    result.type = VOID;

    return result;
}


//////////////////////////////////////////
// ArrayDecl Impelementation
//////////////////////////////////////////
ArrayDecl::ArrayDecl(LexerToken _token) : BinaryOp(_token)
{
}


Result ArrayDecl::eval(RefEnv &env)
{
    // get the bounds
    std::vector<int> bounds;
    NaryOp *blist = (NaryOp*) left();
    for(auto itr = blist->begin(); itr != blist->end(); itr++) {
        bounds.push_back((int) NUM_RESULT((*itr)->eval(env)));
    }

    //build the array and insert it into the environment
    ArrayVar *ar = new ArrayVar(token_to_type(token().token), bounds);
    std::string name = right()->token().lexeme;
    env.declare(name, ARRAY_VAR);
    env[name].val.ptr = ar;

    //return void
    Result result;
    result.type = VOID;
    return result;
}


//////////////////////////////////////////
// ArrayAccess Implementation
//////////////////////////////////////////
ArrayAccess::ArrayAccess(LexerToken _token) : BinaryOp(_token), Accessor(_token)
{
}

Result ArrayAccess::eval(RefEnv &env)
{

    //return void
    Result result;
    result.type = VOID;
    return result;
}


Result& ArrayAccess::eval_ref(RefEnv &env)
{
}


//////////////////////////////////////////
// ArrayIndex Implementation 
//////////////////////////////////////////
// An array index node
ArrayIndex::ArrayIndex(LexerToken _token) : NaryOp(_token)
{
}


Result ArrayIndex::eval(RefEnv &env)
{

    //return void
    Result result;
    result.type = VOID;
    return result;
}


//////////////////////////////////////////
// RecordDef Implementation
//////////////////////////////////////////
RecordDef::RecordDef(LexerToken _token) : NaryOp(_token) 
{
}


Result RecordDef::eval(RefEnv &env)
{

    //return void
    Result result;
    result.type = VOID;
    return result;
}


//////////////////////////////////////////
// RecordAccess Implementation
//////////////////////////////////////////
RecordAccess::RecordAccess(LexerToken _token) : BinaryOp(_token), Accessor(_token)
{
}


Result RecordAccess::eval(RefEnv &env)
{

    //return void
    Result result;
    result.type = VOID;
    return result;
}


Result& RecordAccess::eval_ref(RefEnv &env)
{
}
