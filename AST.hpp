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
        CALL,
        PRINTF,
        // GETCHAR, etc
    };

    static std::string TypeName(Type t) {
        switch (t) {
            case Type::DECLARATION: return "DECLARATION";
            case Type::ASSIGNMENT: return "ASSIGNMENT";
            case Type::BEGIN_BLOCK: return "BEGIN BLOCK";
            case Type::END_BLOCK: return "END BLOCK";
            case Type::IF: return "IF";
            case Type::ELSE: return "ELSE";
            case Type::FOR_1: return "FOR EXPRESSION 1";
            case Type::FOR_2: return "FOR EXPRESSION 2";
            case Type::FOR_3: return "FOR EXPRESSION 3";
            case Type::WHILE: return "WHILE";
            case Type::CALL: return "CALL";
            case Type::PRINTF: return "PRINTF";
            case Type::TOKEN: return "TOKEN";
            case Type::INVALID: return "INVALID";
        };
        return "UNKNOWN AST TYPE";
    }

    Type ty = Type::INVALID;
    treeNode* token = nullptr;
    STNode* symbol = nullptr;
    ASTNode* lc = nullptr;
    ASTNode* rs = nullptr;
};


bool isDatatypeSpecifier(treeNode* t) {
    if (t->tokenType != "IDENTIFIER") {
        return false;
    }
    if (t->line == "int" || t->line == "bool" || t->line == "char") {
        return true;
    }

    return false;
}

class AST {

    ASTNode* root = nullptr;
    ASTNode* tail = nullptr; // leftmost child
    ASTNode* sib = nullptr; // rightmost sibling of tail
    treeNode* next = nullptr; // next CST node to be converted to an AST
    int scopeCount; // increments each time we see a function or procedure
    int scopeDepth; // increments on '{', decrements on '}'

public:
    // add a new leftmost child
    // TODO: support the staircasing of for loops
    void child(ASTNode n) {
        ASTNode* nn = new ASTNode(n);
        if (!root) {
            root = nn;
            tail = root;
            sib = root;
            return;
        }

        tail->lc = nn;
        tail = tail->lc;
        sib = tail;
    }

    // add a new rightmost sibling to the last child
    void sibling(ASTNode n) {
        ASTNode* nn = new ASTNode(n);
        if (!root) {
            root = nn;
            tail = root;
            sib = root;
            return;
        }

        sib->rs = nn;
        sib = sib->rs;
    }


    treeNode* declaration(treeNode* next, const SymbolTable& table) {
        child({ASTNode::Type::DECLARATION});
        tail->symbol = table.resolve(next->line, scopeDepth ? scopeCount : 0);
        next = next->next;
        
        // absorb array declarations
        while (next->tokenType != "IDENTIFIER" && next->tokenType != "COMMA" && next->tokenType != "SEMICOLON") {
            next = next->next;
        }

        return next;
    }

    int opPrecedence(treeNode* n) {
        if (n->tokenType == "BOOLEAN_OR") {
            return 0;
        } else if (n->tokenType == "BOOLEAN_AND") {
            return 1;
        } else if (n->tokenType == "BOOLEAN_EQUAL" || n->tokenType == "NOT_EQUAL") {
            return 2;
        } else if (n->tokenType == "GT" || n->tokenType == "GT_EQUAL" || n->tokenType == "LT" || n->tokenType == "LT_EQUAL") {
            return 3;
        } else if (n->tokenType == "ASSIGNMENT_OPERATOR") {
            return 4;
        } else if (n->tokenType == "PLUS" || n->tokenType == "MINUS") {
            return 5;
        } else if (n->tokenType == "ASTERISK" || n->tokenType == "DIVIDE" || n->tokenType == "MODULO") {
            return 6;
        } else if (n->tokenType == "BOOLEAN_NOT") {
            return 7;
        }
        return -1;
    }

    bool isRightAssociative(treeNode* n) {
        return n->tokenType == "BOOLEAN_NOT";
    }

    enum class ExpressionType {
        BOOLEAN,
        NUMERIC,
        VOID,
    };

    std::optional<ExpressionType> opType(treeNode* n) {
        int p = opPrecedence(n);
        if (p > 4 && p != 7) {
            return ExpressionType::NUMERIC;
        } else if (p != -1 && p != 4) {
            return ExpressionType::BOOLEAN;
        }

        return {};
    }

    ExpressionType addExpression(const SymbolTable& table) {
        ExpressionType ty = ExpressionType::VOID;
        std::vector<treeNode*> stack;
        while (next) {
            int prec = opPrecedence(next);
            if (next->tokenType == "IDENTIFIER" && next->line == "TRUE" || next->line == "FALSE") {
                // boolean literal

                sibling({ASTNode::Type::TOKEN, next});
                next = next->next;
                ty = ExpressionType::BOOLEAN;
            } else if (next->tokenType == "IDENTIFIER") {
                // variable, function call, array index

                sibling({ASTNode::Type::TOKEN, next});
                sib->symbol = table.resolve(next->line, scopeDepth ? scopeCount : 0);

                // update datatype
                if (sib->symbol) {
                    if (sib->symbol->dataType == "int" || sib->symbol->dataType == "char") {
                        ty = ExpressionType::NUMERIC;
                    } else if (sib->symbol->dataType == "bool") {
                        ty = ExpressionType::BOOLEAN;
                    } else {
                        ty = ExpressionType::VOID;
                    }
                } else {
                    std::cout << "Unable to find " << next->line << " in symbol table" << std::endl;
                    ty = ExpressionType::VOID;
                }

                next = next->next;

                // array index
                if (next->tokenType == "L_BRACKET") {
                    auto last = next;
                    sibling({ASTNode::Type::TOKEN, next}); // opening bracket
                    next = next->next;

                    if (addExpression(table) != ExpressionType::NUMERIC) {
                        std::cout << "ERROR: array index expression must evaluate to an integer" << std::endl;
                        if (sib->symbol) {
                            std::cout << sib->token->line << ": " << sib->symbol->dataType << std::endl;
                        }
                        
                    }

                    if (!next) {
                        // bug in this function or missing cases in CST
                        std::cout << "INTERNAL ERROR: Statement ended in the middle of a bracket" << std::endl;
                        while (last) {
                            std::cout << last->line << " (" << last->tokenType << ")" << std::endl;
                            last = last->next;
                        }
                    } else {
                        sibling({ASTNode::Type::TOKEN, next}); // closing bracket
                        next = next->next;
                    }

                } else if (next->tokenType == "L_PAREN") {
                    // function call
                    sibling({ASTNode::Type::TOKEN, next}); // opening parenthesis
                    next = next->next;

                    addExpression(table);
                    while (next->tokenType == "COMMA") {
                        // arguments
                        sibling({ASTNode::Type::TOKEN, next}); // comma
                        next = next->next;
                        addExpression(table);
                    }

                    sibling({ASTNode::Type::TOKEN, next}); // closing parenthesis
                    next = next->next;
                }
            } else if (next->tokenType == "INTEGER") {
                // integer literal

                sibling({ASTNode::Type::TOKEN, next});
                next = next->next;
                ty = ExpressionType::NUMERIC;
            } else if (next->tokenType == "SINGLE_QUOTE" || next->tokenType == "DOUBLE_QUOTE") {
                // char and string literals

                sibling({ASTNode::Type::TOKEN, next}); // ' or "
                next = next->next;

                sibling({ASTNode::Type::TOKEN, next}); // char or string or escaped char
                next = next->next;

                sibling({ASTNode::Type::TOKEN, next}); // ' or "
                next = next->next;

                ty = ExpressionType::NUMERIC;
            } else if (prec != -1) {
                // boolean, numeric, and assignment operators

                // pop all greater precedence operators first
                while (!stack.empty() && stack.back()->tokenType != "L_PAREN") {
                    int stackp = opPrecedence(stack.back());

                    if (stackp < prec || stackp == prec && isRightAssociative(stack.back())) {
                        break;
                    }

                    sibling({ASTNode::Type::TOKEN, stack.back()});
                    stack.pop_back();
                }

                stack.push_back(next);
                next = next->next;
            } else if (next->tokenType == "L_PAREN") {
                stack.push_back(next);
                next = next->next;
            } else if (next->tokenType == "R_PAREN") {

                while (!stack.empty() && stack.back()->tokenType != "L_PAREN") {
                    sibling({ASTNode::Type::TOKEN, stack.back()});
                    stack.pop_back();
                }
                // didn't find matching opening parenthesis
                if (stack.empty()) {
                    break;
                }

                stack.pop_back();
                next = next->next;
            } else {
                // not part of the expression
                break;
            }

        }

        if (!next) {
            std::cout << "ERROR: Exiting the numerical expression because we reached the end of the statement" << std::endl;
            std::cout << "\tLikely need to add more cases to the CST parser" << std::endl;
        }
        

        while (!stack.empty()) {
            if (stack.back()->tokenType != "L_PAREN") {
                sibling({ASTNode::Type::TOKEN, stack.back()});
            }
            stack.pop_back();
        }

        
        if (sib->token) {
            auto lastOpType = opType(sib->token);
            if (lastOpType.has_value()) {
                ty = *lastOpType;
            }
        }

        return ty;
    }

    void nextStatement() {
        while (next->next) {
            next = next->next;
        }
        next = next->leftChild;
    }

    AST(const LCRSTree& cst, const SymbolTable& table) {
        next = cst.getRoot();
        scopeCount = 0;
        scopeDepth = 0;
        while (next) {

            // TODO: handle the other AST node types

            if (next->tokenType == "L_BRACE") {
                scopeDepth++;
                child({ASTNode::Type::BEGIN_BLOCK});
            } else if (next->tokenType == "R_BRACE") {
                scopeDepth--;
                child({ASTNode::Type::END_BLOCK});
            } else if (next->line == "function") {
                scopeCount++;
                next = next->next; // data type
                next = next->next; // name
                child({ASTNode::Type::DECLARATION});
                tail->symbol = table.resolve(next->line, scopeCount);
            } else if (next->line == "procedure") {
                scopeCount++;
                next = next->next; // name
                child({ASTNode::Type::DECLARATION});
                tail->symbol = table.resolve(next->line, scopeCount);
            } else if (isDatatypeSpecifier(next)) {
                // DECLARATION

                next = next->next; // ident
                // handle declaration chains
                while (next->tokenType == "IDENTIFIER") {
                    next = declaration(next, table);
                    if (next->tokenType == "COMMA") {
                        next = next->next;
                    }
                }
                
            } else if (next->tokenType == "IDENTIFIER" && next->next && (next->next->tokenType == "ASSIGNMENT_OPERATOR" || next->next->tokenType == "L_BRACKET")) {
                // ASSIGNMENT

                child({ASTNode::Type::ASSIGNMENT});
                tail->symbol = table.resolve(next->line, scopeDepth ? scopeCount : 0);

                if (!tail->symbol) {
                    std::cout << "ERROR: unable to find " << next->line << " in symbol table" << std::endl;
                } else {
                    auto expType = addExpression(table);
                    if (tail->symbol->dataType == "bool") {
                        if (expType != ExpressionType::BOOLEAN) {
                            std::cout << "ERROR: expected boolean expression on line " << next->lineNumber << std::endl;
                        }
                    } else {
                        if (expType != ExpressionType::NUMERIC) {
                            std::cout << "ERROR: expected numeric expression on line " << next->lineNumber << std::endl;
                        }
                    }
                }
                

            } else if (next->line == "if") {
                child({ASTNode::Type::IF});
                next = next->next; // skip 'if' identifier
                next = next->next; // skip opening parenthesis

                auto expType = addExpression(table);
                if (expType != ExpressionType::BOOLEAN) {
                    std::cout << "ERROR: if condition must evaluate to a boolean" << std::endl;
                }

                // version using LCRSTree::booleanExpressionPostFix
                //
                // std::vector<treeNode*> treeNodeList;
                // int parens = 0;
                // while (next) {
                //     if (next->tokenType == "R_PAREN") {
                //         parens--;
                //     } else if (next->tokenType == "L_PAREN") {
                //         parens++;
                //     }

                //     if (parens < 0) {
                //         break;
                //     }

                //     treeNodeList.push_back(next);
                //     next = next->next;
                // }

                // auto tnl = LCRSTree::booleanExpressionPostFix(treeNodeList);
                // for (auto* tr : tnl) {
                //     sibling({ASTNode::Type::TOKEN, tr});
                // }

            } else if (next->line == "while") {
                child({ASTNode::Type::WHILE});
                next = next->next; // skip 'while' identifier
                next = next->next; // skip opening parenthesis

                auto expType = addExpression(table);
                if (expType != ExpressionType::BOOLEAN) {
                    std::cout << "ERROR: while condition must evaluate to a boolean" << std::endl;
                }

            }

            // skip trailing tokens (semicolons, etc)
            if (next) {
                nextStatement();
            }
        }

    }

    void print() {
        ASTNode* n = root;
        if (!n) {
            std::cout << "Empty AST\n";
            return;
        }
        // ignore the for loop case for now
        do {
            // std::cout << std::endl;
            std::cout << ASTNode::TypeName(n->ty);
            ASTNode* sibs = n->rs;
            while (sibs) {
                std::cout << "   " << sibs->token->line;
                sibs = sibs->rs;
            }
            std::cout << std::endl;
            // std::cout << "Symbol: " << (n->symbol ? n->symbol->identifierName : "NO SYMBOL") << std::endl;
        } 
        while (n = n->lc);
    }

};
