#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "AST.hpp"

class Interpreter {
    // tail is a pointer to the bottom most AST node, that has a child pointer
    // next is a pointer to the right most AST node, that has a sibling pointer
    //
    // example:
    //
    // DECLARATION
    // BEGIN BLOCK
    // DECLARATION
    // ASSIGNMENT   sum   0   =
    // ^tail              ^next
    // IF   n   1   >=
    ASTNode* next;
    ASTNode* tail;

    // {tail, next} pair that should be restored after calling a function
    // not really necessary, since most of the control flow uses the C++ call stack instead
    std::stack<std::pair<ASTNode*, ASTNode*>> call_stack;

    SymbolTable* table;
public:
    Interpreter(SymbolTable* table) {
        this->table = table;
        // find the main symbol, its declaration in the AST, and 
        // proceed two lines down to the first statement in the body
        execute(table->resolve("main", 0)->decl->lc->lc);
    }

    // call with next pointing to the identifier of the function call
    // pushes to the call stack, executes the function, and restores the call stack after
    int call();
    
    // call with tail pointing to the line below the BEGIN_BLOCK
    void skip_block();

    // evaluate the code in this block, starting from the ASTNode* after the BEGIN_BLOCK
    int execute(ASTNode* start);

    // call with next pointing to the first ASTNode in the expression
    void assignment_expression();
    int expression();
};


#endif /* INTERPRETER_HPP */
