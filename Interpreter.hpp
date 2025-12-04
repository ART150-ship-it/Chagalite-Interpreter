#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "AST.hpp"

class Interpreter {
    SymbolTable* table;
    ASTNode* next;
    ASTNode* tail;

    std::stack<std::pair<ASTNode*, ASTNode*>> call_stack;
public:
    bool isExpression() {
        return next->ty == ASTNode::Type::TOKEN;
    }

    Interpreter(SymbolTable* table) {
        this->table = table;
        execute(table->resolve("main", 0));
    }

    int call() {
        ASTNode* func = next;
        next = next->rs; // L_PAREN
        next = next->rs; // first param

        std::vector<int> params;

        while (next->token->tokenType != "R_PAREN") {
            params.push_back(expression());
            if (next && next->token->tokenType == "COMMA") {
                next = next->rs;
            }
        }

        // next is R_PAREN
        next = next->rs; // first after R_PAREN

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
        call_stack.push({tail, next});
        return execute(func->symbol);
        // V DECLARATION
        // V OPEN BRACE
        // -> ...
    }

    void execute_statement() {}


    // evaluate the code in this function, returning the value that function returned
    int execute(STNode* symbol) {
        
        // tail is a pointer to the bottom most AST node, that has a child pointer
        // next is a pointer to the right most AST node, that has a sibling pointer

        /* example
        DECLARATION
        BEGIN BLOCK
        DECLARATION
        ASSIGNMENT   sum   0   =
        ^tail              ^next
        IF   n   1   >=
        */

        

        // jump to the AST node after the BEGIN BLOCK of the function with this symbol
        tail = symbol->decl->lc->lc;

        if (!tail) {
            std::cout << "Error: unable to find declaration of symbol \"" << symbol->identifierName << "\"\n";
            exit(1);
        }

        // std::cout << "executing node of type " << ASTNode::TypeName(tail->ty) << std::endl;

        int blocks = 1;
        while (tail && blocks) {
            
            next = tail;
            if (next->ty == ASTNode::Type::BEGIN_BLOCK) {
                blocks++;
            } else if (next->ty == ASTNode::Type::END_BLOCK) {
                blocks--;
            } else if (next->ty == ASTNode::Type::PRINTF) {
                std::vector<int> args;
                next = next->rs;
                std::string fmt = next->token->line;
                printf("%s ", fmt.c_str());
                while (next->rs) {
                    next = next->rs;

                    printf("%d, ", *next->symbol->value);
                }
                printf("\n");
            } else if (next->ty == ASTNode::Type::CALL) {
                next = next->rs;
                call();
            } else if (next->ty == ASTNode::Type::ASSIGNMENT) {
                next = next->rs;
                // next now points to identifier being assigned
                STNode* stn = next->symbol;
                int index = 0;
                if (stn->datatypeIsArray == "no") {
                    if (!stn->value) {
                        stn->value = new int;
                    }
                } else {
                    if (!stn->value) {
                        stn->value = new int[stn->datatypeArraySize];
                    }
                    next = next->rs; // now points to open bracket
                    next = next->rs;
                    index = expression();
                    std::cout << "assignment to " << stn->identifierName << " with index " << index << std::endl;
                }
                next = tail->rs;
                stn->value[index] = expression();
            } else if (next->ty == ASTNode::Type::IF) {
                next = next->rs;
                // next now points to condition
                int cond = expression();

                if (!cond) {
                    // skip content
                    // skip over BEGIN_BLOCK
                    tail = tail->lc;
                    int inner = blocks + 1;
                    while (inner != blocks) {
                        tail = tail->lc;
                        if (tail->ty == ASTNode::Type::BEGIN_BLOCK) inner++;
                        if (tail->ty == ASTNode::Type::END_BLOCK) inner--;
                    }
                }

            } else if (next->ty == ASTNode::Type::RETURN) {
                if (next->rs) {
                    // function
                    next = next->rs;
                    int retval = expression();
                    // restore program counter
                    auto tn = call_stack.top();
                    call_stack.pop();
                    tail = tn.first;
                    next = tn.second;
                    return retval;
                } else {
                    // procedure
                    break;
                }
            }
            tail = tail->lc;
        }
        // procedure
        if (call_stack.empty()) {
            // main
        } else {
            // restore program counter
            auto tn = call_stack.top();
            call_stack.pop();
            tail = tn.first;
            next = tn.second;
        }
        return 0;
    }

    

    int expression() {
        if (!isExpression()) {
            std::cerr << "Error: Cannot evaluate non-token node of type " << ASTNode::TypeName(next->ty) << std::endl;
            exit(1);
        }

        std::stack<int> st;
        do {
            if (next->token->tokenType == "IDENTIFIER" && next->rs && next->rs->token->tokenType == "L_PAREN") {
                // function call
                st.push(call());
                
            } else if (next->token->tokenType == "IDENTIFIER" && next->rs && next->rs->token->tokenType == "L_BRACKET") {
                // array index
                int* arr = next->symbol->value;

                if (!arr) {
                    arr = new int[next->symbol->datatypeArraySize];
                }

                next = next->rs; // left bracket
                next = next->rs; // first symbol in index
                int index = expression();
                st.push(arr[index]);
                std::cout << "Indexed array in expression with value " << st.top() << " at index " << index << std::endl;
                next = next->rs; // first symbol after right bracket
            } else if (next->token->tokenType == "IDENTIFIER") {
                // variable
                if (!next->symbol->value) {
                    next->symbol->value = new int(0);
                }
                st.push(*next->symbol->value);
                next = next->rs;
            } else if (next->token->tokenType == "INTEGER") {
                st.push(std::stoi(next->token->line));
                next = next->rs;
            } else if (next->token->tokenType == "PLUS") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a + b);
                next = next->rs;
            } else if (next->token->tokenType == "MINUS") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a - b);
                next = next->rs;
            } else if (next->token->tokenType == "DIVIDE") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a / b);
                next = next->rs;
            } else if (next->token->tokenType == "ASTERISK") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a * b);
                next = next->rs;
            } else if (next->token->tokenType == "MODULO") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a % b);
                next = next->rs;
            } else if (next->token->tokenType == "BOOLEAN_AND") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a && b);
                next = next->rs;
            } else if (next->token->tokenType == "BOOLEAN_OR") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a || b);
                next = next->rs;
            } else if (next->token->tokenType == "BOOLEAN_NOT") {
                int a = st.top(); st.pop();
                st.push(!a);
                next = next->rs;
            } else if (next->token->tokenType == "BOOLEAN_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a == b);
                next = next->rs;
            } else if (next->token->tokenType == "NOT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a != b);
                next = next->rs;
            } else if (next->token->tokenType == "GT") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a > b);
                next = next->rs;
            } else if (next->token->tokenType == "GT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a >= b);
                next = next->rs;
            } else if (next->token->tokenType == "LT") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a < b);
                next = next->rs;
            } else if (next->token->tokenType == "LT_EQUAL") {
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(a <= b);
                next = next->rs;
            } else if (next->token->tokenType == "ASSIGNMENT_OPERATOR") {
                // doesn't actually do anything, assignments handled on the calling side
                int b = st.top(); st.pop();
                int a = st.top(); st.pop();
                st.push(b);
                next = next->rs;
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
