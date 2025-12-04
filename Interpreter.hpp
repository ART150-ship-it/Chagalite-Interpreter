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
        execute(table->resolve("main", 0)->decl->lc->lc);
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
        int retval = execute(func->symbol->decl->lc->lc);

        auto tn = call_stack.top();
        call_stack.pop();
        tail = tn.first;
        next = tn.second;

        return retval;
    }

    
    void assignment_expression() {
        STNode* sym = next->symbol;
        ASTNode* start = next;
        int index = 0;
        int val = expression();
        ASTNode* end = next;
        next = start;
        if (sym->datatypeIsArray == "no") {
            if (!sym->value) {
                sym->value = new int;
            }
        } else if (next->rs->token->tokenType == "L_BRACKET") {
            
            if (!sym->value) {
                sym->value = new int[sym->datatypeArraySize];
            }
            next = next->rs; // now points to open bracket
            next = next->rs;
            index = expression();
        } else {
            // string assignment
            index = -1;
        }
        if (index >= 0) {
            // scalar assignment
            next = start;
            sym->value[index] = val;
        } else {
            // array assignment

            next = start->rs->rs; // now string

            std::string str = next->token->line; // IDENT " string " =
            // parse escaped characters in string
            for (int i = 0, j = 0; i < str.length(); i++, j++) {
                if (str[i] == '\\') {
                    i++;
                    if (str[i] == 'x') {
                        i++;
                        if (str[i] >= 'a' && str[i] <= 'f') sym->value[j] = str[i] - 'a' + 10;
                        else if (str[i] >= 'A' && str[i] <= 'F') sym->value[j] = str[i] - 'A' + 10;
                        else if (str[i] >= '0' && str[i] <= '9') sym->value[j] = str[i] - '0';
                        
                    } else if (str[i] == 'n') {
                        sym->value[j] = '\n';
                    }
                } else {
                    sym->value[j] = str[i];
                }
                
            }
        }

        next = end;

    }

    // call with tail = line below the BEGIN_BLOCK
    void skip_block() {
        int block = 1;

        // auto step = [&]{

        // };
        // std::cout << "start skip from " << ASTNode::TypeName(tail->ty) << "\n";
        while (block) {
            if (tail->ty == ASTNode::Type::BEGIN_BLOCK) block++;
            if (tail->ty == ASTNode::Type::END_BLOCK)   block--;
            if (!tail->lc) {
                while (tail->rs) {
                    tail = tail->rs;
                }
            }
            tail = tail->lc;
        }
        // std::cout << "end skip on " << ASTNode::TypeName(tail->ty) << "\n";
    }


    // evaluate the code in this block, starting from the ASTNode* after the BEGIN_BLOCK
    int execute(ASTNode* start) {
        
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
        tail = start;

        int blocks = 1;
        while (tail && blocks) {
            
            next = tail;
            if (next->ty == ASTNode::Type::BEGIN_BLOCK) {
                blocks++;
                tail = tail->lc;
            } else if (next->ty == ASTNode::Type::END_BLOCK) {
                blocks--;
                if (tail)
                    tail = tail->lc;
            } else if (next->ty == ASTNode::Type::PRINTF) {
                std::vector<ASTNode*> args;
                next = next->rs;
                std::string fmt = next->token->line;
                while (next->rs) {
                    next = next->rs;
                    args.push_back(next);
                }
                
                size_t pos = 0;
                int argIdx = 0;

                while ((pos = fmt.find("%", pos)) != std::string::npos) {
                    std::string value;
                    if (fmt[pos + 1] == 'd') {
                        value = std::to_string(*args[argIdx++]->symbol->value);
                    } else {
                        for (int i = 0; i < args[argIdx]->symbol->datatypeArraySize; i++) {
                            if (args[argIdx]->symbol->value[i] == 0) {
                                break;
                            }
                            value.push_back(args[argIdx]->symbol->value[i]);
                        }
                        argIdx++;
                    }
                    
                    fmt.replace(pos, 2, value);
                    pos += value.size();
                }

                size_t newLinePos = fmt.find("\\n");
                if (newLinePos != std::string::npos) {
                    fmt.replace(newLinePos, 2, "\n");
                }

                std::cout << fmt;
                tail = tail->lc;
            } else if (next->ty == ASTNode::Type::CALL) {
                next = next->rs;
                call();
                tail = tail->lc;
            } else if (next->ty == ASTNode::Type::WHILE) {
                next = next->rs;
                ASTNode* cond = next;
                ASTNode* loop_start = tail->lc->lc;
                while (true) {
                    next = cond;
                    if (!expression()) {
                        break;
                    }
                    execute(loop_start);
                }
                
            } else if (next->ty == ASTNode::Type::FOR_1) {
                // init assignment
                if (next->rs) {
                    next = next->rs;
                    assignment_expression();
                }
                while (tail->rs) tail = tail->rs;
                tail = tail->lc; // FOR EXPRESSION 2
                ASTNode* cond = tail->rs;
                
                while (tail->rs) tail = tail->rs;
                tail = tail->lc; // FOR EXPRESSION 3
                ASTNode* update = tail->rs;

                while (tail->rs) tail = tail->rs;
                ASTNode* body = tail->lc->lc;

                while (true) {
                    next = cond;
                    if (!expression()) {
                        break;
                    }

                    execute(body);

                    next = update;
                    assignment_expression();
                }


            } else if (next->ty == ASTNode::Type::ASSIGNMENT) {
                next = next->rs;
                // next now points to identifier being assigned
                assignment_expression();
                tail = tail->lc;
            } else if (next->ty == ASTNode::Type::IF) {
                next = next->rs;
                // next now points to condition
                int cond = expression();
                tail = tail->lc; // BEGIN BLOCK
                tail = tail->lc; // first statement of body
                // std::cout << "SEE IF\n";
                // skip_block();
                // tail = tail->lc;
                
                
                if (cond) {
                    // std::cout << "Executing if body starting with " << ASTNode::TypeName(tail->ty) << "\n";

                    execute(tail);

                    // skip_block();
                    if (tail->ty == ASTNode::Type::ELSE) {
                        tail = tail->lc->lc;
                        skip_block();
                    }
                } else {
                    skip_block();
                    if (tail->ty == ASTNode::Type::ELSE) {
                        tail = tail->lc->lc;
                        // std::cout << "Executing else body starting with " << ASTNode::TypeName(tail->ty) << "\n";
                        execute(tail);
                        // skip_block();
                    }
                }
                // tail = tail->lc;

            } else if (next->ty == ASTNode::Type::RETURN) {
                if (next->rs) {
                    // function
                    next = next->rs;
                    int retval = expression();

                    return retval;
                } else {
                    // procedure
                    break;
                }
                tail = tail->lc;
            } else {
                if (tail)
                    tail = tail->lc;
            }
                
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
                    next->symbol->value = arr;
                }

                next = next->rs; // left bracket
                next = next->rs; // first symbol in index
                int index = expression();
                st.push(arr[index]);
                next = next->rs; // first symbol after right bracket
            } else if (next->token->tokenType == "IDENTIFIER") {
                if (next->token->line == "TRUE") {
                    st.push(1);
                } else if (next->token->line == "FALSE") {
                    st.push(0);
                } else {
                    // variable
                    if (!next->symbol->value) {
                        next->symbol->value = new int(0);
                    }
                    st.push(*next->symbol->value);
                }

                next = next->rs;
            } else if (next->token->tokenType == "INTEGER") {
                st.push(std::stoi(next->token->line));
                next = next->rs;
            } else if (next->token->tokenType == "SINGLE_QUOTE") {
                next = next->rs;
                st.push(next->token->line[0]);
                next = next->rs;
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
            return -333333;
        }

        return st.top();
    }

};


#endif /* INTERPRETER_HPP */
