#pragma once

#include "STLL.hpp"
#include <iostream>
#include <optional>

struct ASTNode {
    enum class Type {
        INVALID,
        TOKEN,
        DECLARATION,
        ASSIGNMENT,
        BEGIN_BLOCK,
        END_BLOCK,
        IF,
        ELSE,
        FOR_1,
        FOR_2,
        FOR_3,
        WHILE,
        PRINTF,
        RETURN,
        CALL,
        // GETCHAR, etc
    };

    static std::string TypeName(Type t);

    Type ty = Type::INVALID;
    treeNode* token = nullptr;
    STNode* symbol = nullptr;
    ASTNode* lc = nullptr;
    ASTNode* rs = nullptr;
};

// class to parse the CST into an AST
// with the exception of the for loop, consumes once "line" of the CST per AST node
class AST {

    ASTNode* root = nullptr; 
    ASTNode* tail = nullptr; // leftmost child
    ASTNode* sib = nullptr; // rightmost sibling of tail
    treeNode* next = nullptr; // next CST node to be converted to an AST
    int scopeCount; // increments each time we see a function or procedure
    int scopeDepth; // increments on '{', decrements on '}'

    // add a new leftmost child
    void child(ASTNode n);

    // add a new rightmost sibling to the last child
    void sibling(ASTNode n);

    // DECLARATION branch of the main if-else chain, extracted into its own function to reduce indentation
    treeNode* declaration(treeNode* next, const SymbolTable& table);

    void internalError(std::string msg) {
        std::cout << "INTERNAL ERROR on line " << next->lineNumber << ": " << msg << std::endl;
    }

    void syntaxError(std::string msg) {
        std::cout << "Syntax error on line " << next->lineNumber << ": " << msg << std::endl;
    }

    // consume the next CST node and convert it to an AST node, adding a reference
    // to its symbol in the symbol table if it was an identifier
    void advanceSibling(const SymbolTable& table);

    // same as advanceSibling(), but checks that the next CST node is of a specific token type
    void expectSibling(const SymbolTable& table, std::string tokenType);

    // move the CST pointer to the next line
    void nextStatement();

    void printNode(ASTNode* n);

public:

    enum class ExpressionType {
        BOOLEAN,
        NUMERIC,
        VOID,
    };

    bool isDatatypeSpecifier(treeNode* t);
    std::optional<ExpressionType> opType(treeNode* n);
    bool isRightAssociative(treeNode* n);
    int opPrecedence(treeNode* n);



    // build an AST from the CST and symbol table
    AST(const LCRSTree& cst, const SymbolTable& table);

    /**
     * @brief Function to consume an expression from the CST and add it to the AST in postfix notation
     * 
     * Expressions can be numeric, boolean, or have no evaluation type (VOID). 
     * An example of a VOID expression is an assignment
     * 
     * Handles nested expressions in array indices and function calls
     * 
     * Identifiers are linked with their symbol in the symbol table
     * 
     * The only type checking performed is to ensure array index expressions are numeric
     * 
     * @param table the symbol table to register identifiers in
     * @return The type the parsed expression evaluates to
     */
    ExpressionType addExpression(const SymbolTable& table);

    // BFS-ish traversal
    // good enough since at most one child will have a child of its own
    void print() {
        printNode(root);
    }

};
