#include "AST.hpp"

bool isDatatypeSpecifier(treeNode* t) {
    if (t->tokenType != "IDENTIFIER") {
        return false;
    }
    if (t->line == "int" || t->line == "bool" || t->line == "char") {
        return true;
    }

    return false;
}

AST::AST(const LCRSTree& cst, const SymbolTable& table) {
    next = cst.getRoot();
    scopeCount = 0;
    scopeDepth = 0;
    while (next) {
        // at the start of the loop, `next` is the first token in the CST line 

        // one if-else block for each AST node
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

        } else if (next->line == "while") {
            child({ASTNode::Type::WHILE});
            next = next->next; // skip 'while' identifier
            next = next->next; // skip opening parenthesis

            auto expType = addExpression(table);
            if (expType != ExpressionType::BOOLEAN) {
                std::cout << "ERROR: while condition must evaluate to a boolean" << std::endl;
            }

        } else if (next->line == "else") {
            child({ASTNode::Type::ELSE});
        } else if (next->line == "for") {
            child({ASTNode::Type::FOR_1});
            next = next->next; // skip 'for' identifier
            next = next->next; // skip opening parenthesis
            auto forInitType = addExpression(table);
            next = next->next; // skip semicolon
            tail = sib; // stair case

            child({ASTNode::Type::FOR_2});
            auto forCondType = addExpression(table);
            next = next->next; // skip semicolon
            tail = sib; // stair case

            child({ASTNode::Type::FOR_3});
            auto forUpdateType = addExpression(table);
            tail = sib; // stair case
        } else if (next->line == "printf") {
            child({ASTNode::Type::PRINTF});
            next = next->next; // skip printf
            next = next->next; // skip opening parenthesis
            next = next->next; // skip quote
            expectSibling(table, "STRING");
            next = next->next; // skip quote
            while (next->tokenType != "R_PAREN") {
                // shouldn't do anything if there is no expression to add
                addExpression(table);
                if (next->tokenType == "COMMA") {
                    next = next->next;
                }
            }
        } else if (next->line == "return") {
            child({ASTNode::Type::RETURN});
            next = next->next; // skip return
            addExpression(table);
        } else if (next->tokenType == "IDENTIFIER" && next->next->tokenType == "L_PAREN") {
            child({ASTNode::Type::CALL});
            addExpression(table);
        }

        // skip trailing tokens (semicolons, else, closing parens, etc)
        if (next) {
            nextStatement();
        }
    }

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

std::optional<AST::ExpressionType> opType(treeNode* n) {
    int p = opPrecedence(n);
    if (p > 4 && p != 7) {
        return AST::ExpressionType::NUMERIC;
    } else if (p != -1 && p != 4) {
        return AST::ExpressionType::BOOLEAN;
    }

    return {};
}

AST::ExpressionType AST::addExpression(const SymbolTable& table) {
    ExpressionType ty = ExpressionType::VOID;
    std::vector<treeNode*> stack;
    while (next) {
        int prec = opPrecedence(next);
        if (next->tokenType == "IDENTIFIER" && (next->line == "TRUE" || next->line == "FALSE")) {
            // boolean literal

            advanceSibling(table);
            ty = ExpressionType::BOOLEAN;
        } else if (next->tokenType == "IDENTIFIER") {
            // variable, function call, array index
            advanceSibling(table);

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
                syntaxError("unable to find " + sib->token->line + " in symbol table");
                ty = ExpressionType::VOID;
            }

            if (next->tokenType == "L_BRACKET") {
                // array index
                auto last = next;
                advanceSibling(table);

                if (addExpression(table) != ExpressionType::NUMERIC) {
                    syntaxError("array index expression must be numeric");
                }

                expectSibling(table, "R_BRACKET");
                
            } else if (next->tokenType == "L_PAREN") {
                // function call
                advanceSibling(table);

                addExpression(table);
                while (next->tokenType == "COMMA") {
                    // arguments
                    advanceSibling(table); // comma

                    addExpression(table);
                }

                expectSibling(table, "R_PAREN");
            }
        } else if (next->tokenType == "INTEGER") {
            // integer literal

            advanceSibling(table);
            
            ty = ExpressionType::NUMERIC;
        } else if (next->tokenType == "SINGLE_QUOTE" || next->tokenType == "DOUBLE_QUOTE") {
            // char and string literals

            advanceSibling(table); // ' or "
            advanceSibling(table); // char or string or escaped char
            advanceSibling(table); // ' or "

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
        internalError("Exiting the numerical expression unexpectedly due to end of statement.\nLikely need to add more cases to the CST parser");
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



void AST::printNode(ASTNode* n) {
    if (!n) {
        return;
    }
    if (n->ty == ASTNode::Type::TOKEN) {
        std::cout << "   " << n->token->line;
    } else {
        std::cout << ASTNode::TypeName(n->ty);
    }
    if (!n->rs) {
        std::cout << std::endl;
    }
    printNode(n->rs);
    printNode(n->lc);
}

// add a new leftmost child
void AST::child(ASTNode n) {
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
void AST::sibling(ASTNode n) {
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

treeNode* AST::declaration(treeNode* next, const SymbolTable& table) {
    child({ASTNode::Type::DECLARATION});
    tail->symbol = table.resolve(next->line, scopeDepth ? scopeCount : 0);
    next = next->next;
    
    // absorb array declarations
    while (next->tokenType != "IDENTIFIER" && next->tokenType != "COMMA" && next->tokenType != "SEMICOLON") {
        next = next->next;
    }

    return next;
}

void AST::advanceSibling(const SymbolTable& table) {
    sibling({ASTNode::Type::TOKEN, next});
    if (next->tokenType == "IDENTIFIER") {
        sib->symbol = table.resolve(next->line, scopeDepth ? scopeCount : 0);
    }
    if (next->next) {
        next = next->next;
    } else {
        internalError("Reached end of statement unexpectedly.\nLikely need to add more cases to the CST parser.");
    }
}

// same as advanceSibling(), but checks that the next CST node is of a specific token type
void AST::expectSibling(const SymbolTable& table, std::string tokenType) {
    if (next->tokenType != tokenType) {
        internalError(std::string("Expected ") + tokenType + std::string(", got ") + next->tokenType);
    }
    advanceSibling(table);
}

// move the CST pointer to the next line
void AST::nextStatement() {
    while (next->next) {
        next = next->next;
    }
    next = next->leftChild;
}

std::string ASTNode::TypeName(Type t) {
    switch (t) {
        case Type::INVALID: return "INVALID";
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
        case Type::PRINTF: return "PRINTF";
        case Type::RETURN: return "RETURN";
        case Type::CALL: return "CALL";
        case Type::TOKEN: return "TOKEN"; 
    };
    return "UNKNOWN AST TYPE";
}