#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "AST.hpp"

class Interpreter {
    AST ast;
    ASTNode* next;

    bool isExpression() {
        return next->ty == ASTNode::Type::TOKEN;
    }

    

    int call() {
        ASTNode* func = next;
        next = next->lc; // L_PAREN
        next = next->lc; // first param

        std::vector<int> params;

        while (next->token->tokenType != "R_PAREN") {
            params.push_back(expression());
            if (next && next->token->tokenType == "COMMA") {
                next = next->lc;
            }
        }

        if (params.size() != func->symbol->params.size()) {
            std::cerr << "Error: mismatched parameter list sizes. Attempted to call " << func->symbol->identifierName << " with " << params.size() << " arguments\n";
            exit(1);
        }

        for (size_t i = 0; i < params.size(); i++) {
            STNode* p = func->symbol->params[i];
            delete p->value;
            p->value = new int(params[i]);
        }

        // jump

    }

    int expression() {
        if (!isExpression()) {
            std::cerr << "Error: Cannot evaluate non-token node of type " << ASTNode::TypeName(next->ty) << std::endl;
            exit(1);
        }

        std::stack<int> st;
        do {
            if (next->token->tokenType == "IDENTIFIER" && next->lc && next->lc->token->tokenType == "L_PAREN") {
                // function call
                st.push(call());
                
            } else if (next->token->tokenType == "IDENTIFIER" && next->lc && next->lc->token->tokenType == "L_BRACKET") {
                // array index
                int* arr = next->symbol->value;
                next = next->lc; // left bracket
                next = next->lc; // first symbol in index
                st.push(arr[expression()]);
                next = next->lc; // first symbol after right bracket
            } else if (next->token->tokenType == "IDENTIFIER") {
                // variable
                st.push(*next->symbol->value);
            } else if (next->token->tokenType == "INTEGER") {
                st.push(std::stoi(next->token->line));
            } else if (next->token->tokenType == "PLUS") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a + b);
            } else if (next->token->tokenType == "MINUS") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a - b);
            } else if (next->token->tokenType == "DIVIDE") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a / b);
            } else if (next->token->tokenType == "ASTERISK") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a * b);
            } else if (next->token->tokenType == "MODULO") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a % b);
            } else if (next->token->tokenType == "BOOLEAN_AND") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a && b);
            } else if (next->token->tokenType == "BOOLEAN_OR") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a || b);
            } else if (next->token->tokenType == "BOOLEAN_NOT") {
                int a = st.top(); st.pop();
                st.push(!a);
            } else if (next->token->tokenType == "BOOLEAN_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a == b);
            } else if (next->token->tokenType == "NOT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a != b);
            } else if (next->token->tokenType == "GT") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a > b);
            } else if (next->token->tokenType == "GT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a >= b);
            } else if (next->token->tokenType == "LT") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a < b);
            } else if (next->token->tokenType == "LT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a <= b);
            } else if (next->token->tokenType == "ASSIGNMENT_OPERATOR") {
                std::cerr << "Error: assignment operator not allowed in general expressions. The right hand side should be consumed before calling expression()\n";
                exit(1);
            } else {
                break;
            }
        } while (next);

        if (st.empty()) {
            return -33;
        }

        return st.top();
    }

};


#endif /* INTERPRETER_HPP */
