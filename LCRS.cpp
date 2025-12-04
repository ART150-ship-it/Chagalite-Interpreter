#include <iostream>
#include "LCRS.hpp"


LCRSTree::LCRSTree()
{

}

LCRSTree::LCRSTree(const linklist& other)
{
    Node* walker = other.getFirst();
    identifer(walker);
}

LCRSTree::LCRSTree(const LCRSTree& otherTree)
{
    //do nothing
}

void LCRSTree::identifer(Node* currentToken)
{
    if(last != nullptr){
        if(last->line == "{" or (last->line == ";" and isForLoop == false) or last->line == "}"){
            addLeftChild(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
        }
        else{
            addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
        }
    }
    else{
        addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    }
    if(currentToken->theToken == "for"){
        isForLoop = true;
    }
    //Token has been placed in AST
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        // if(reservedWordChecker(currentToken->theToken) != ""){
        if(reservedWordChecker(currentToken->theToken) != "" and currentToken->next->next->theToken != "\""){
            std::cout << "Syntax error on line " << currentToken->lineNumber  + 1 << ": reserved word \"" << currentToken->theToken << "\" cannot be used for the name of a function." << std::endl;
            exit(2);
        }
        L_PAREN(currentToken->next);
    }
    else if(nextTokenType == "R_PAREN"){
        //if(reservedWordChecker(currentToken->theToken) != ""){
        if(reservedWordChecker(currentToken->theToken) != "" and reservedWordChecker(currentToken->theToken) != "void"){
            std::cout << "Syntax error on line " << currentToken->lineNumber + 1 << ": reserved word \"" << currentToken->theToken << "\" cannot be used for the name of a variable." << std::endl;
            exit(2);
        }
        R_PAREN(currentToken->next);
    }
    else if(nextTokenType == "L_BRACE"){
        L_BRACE(currentToken->next);
    }
    else if(nextTokenType == "L_BRACKET"){
        L_BRACKET(currentToken->next);
    }
    else if(nextTokenType == "R_BRACKET"){
        R_BRACKET(currentToken->next);
    }
    else if(nextTokenType == "SEMICOLON"){
        if(reservedWordChecker(currentToken->theToken) != ""){
            std::cout << "Syntax error on line " << currentToken->lineNumber + 1 << ": reserved word \"" << currentToken->theToken << "\" cannot be used for the name of a variable." << std::endl;
            exit(2);
        }
        SEMICOLON(currentToken->next);
    }
    else if(nextTokenType == "ASSIGNMENT_OPERATOR"){
        ASSIGNMENT_OPERATOR(currentToken->next);
    }   
    else if(nextTokenType == "GT_EQUAL"){
        GT_EQUAL(currentToken->next);
    }
    else if(nextTokenType == "GT"){
        GT(currentToken->next);
    }
    else if(nextTokenType == "LT"){
        LT(currentToken->next);
    }
    else if(nextTokenType == "LT_EQUAL"){
        LT_EQUAL(currentToken->next);
    }
    else if(nextTokenType == "ASTERISK"){
        ASTERISK(currentToken->next);
    }  
    else if(nextTokenType == "PLUS"){
        PLUS(currentToken->next);
    }
    else if(nextTokenType == "MINUS"){
        MINUS(currentToken->next);
    }    
    else if(nextTokenType == "COMMA"){
        COMMA(currentToken->next);
    }
    else if(nextTokenType == "MODULO"){
        MODULO(currentToken->next);
    }   
    else if(nextTokenType == "BOOLEAN_EQUAL"){
        BOOLEAN_EQUAL(currentToken->next);
    }
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
}

void LCRSTree::L_PAREN(Node *currentToken)
{
    if(last != nullptr){
        if(last->line == "{" or (last->line == ";" and isForLoop == false) or last->line == "}"){
            addLeftChild(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
        }
        else{
            addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
        }
    }
    else{
        addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    }    
    //addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "DOUBLE_QUOTE"){
        DOUBLE_QUOTE(currentToken->next);
    }
    else if(nextTokenType == "BOOLEAN_NOT"){
        BOOLEAN_NOT(currentToken->next);
    }
}

void LCRSTree::R_PAREN(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "L_BRACE"){
        L_BRACE(currentToken->next);
    }
    else if(nextTokenType == "R_PAREN"){
        R_PAREN(currentToken->next);
    }
    else if(nextTokenType == "R_BRACKET"){
        R_BRACKET(currentToken->next);
    }
    else if(nextTokenType == "R_BRACE"){
        R_BRACE(currentToken->next);
    }
    else if(nextTokenType == "ASTERISK"){
        ASTERISK(currentToken->next);
    }    
    else if(nextTokenType == "DIVIDE"){
        DIVIDE(currentToken->next);
    }    
    else if(nextTokenType == "SEMICOLON"){
        SEMICOLON(currentToken->next);
    }
    else if(nextTokenType == "BOOLEAN_EQUAL"){
        BOOLEAN_EQUAL(currentToken->next);
    }
    else if(nextTokenType == "BOOLEAN_AND"){
        BOOLEAN_AND(currentToken->next);
    }    
}

void LCRSTree::L_BRACE(Node *currentToken)
{
    addLeftChild(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(isForLoop == true){
        isForLoop = false;
    }
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "R_BRACE"){
        R_BRACE(currentToken->next);
    }
}

void LCRSTree::R_BRACE(Node *currentToken)
{
    addLeftChild(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "R_BRACE"){
        R_BRACE(currentToken->next);
    }
    //if nextTokenType is "" then we are done    
}

void LCRSTree::L_BRACKET(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "INTEGER"){
        if(currentToken->next->theToken[0] == '-'){
            std::cout << "Syntax error on line " << currentToken->lineNumber + 1 << ": array declaration size must be a positive integer." << std::endl;
            exit(2);
        }
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "PLUS"){
        PLUS(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::R_BRACKET(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "SEMICOLON"){
        SEMICOLON(currentToken->next);
    }
    else if(nextTokenType == "R_PAREN"){
        R_PAREN(currentToken->next);
    }
    else if(nextTokenType == "BOOLEAN_EQUAL"){
        BOOLEAN_EQUAL(currentToken->next);
    }
    else if(nextTokenType == "ASSIGNMENT_OPERATOR"){
        ASSIGNMENT_OPERATOR(currentToken->next);
    }
    else if(nextTokenType == "COMMA"){
        COMMA(currentToken->next);
    }
    else if(nextTokenType == "PLUS"){
        PLUS(currentToken->next);
    }
    else if(nextTokenType == "MINUS"){
        MINUS(currentToken->next);
    }
    else if(nextTokenType == "ASTERISK"){
        ASTERISK(currentToken->next);
    }
    else if(nextTokenType == "DIVIDE"){
        DIVIDE(currentToken->next);
    }
    else if(nextTokenType == "MODULO"){
        MODULO(currentToken->next);
    }
}

void LCRSTree::SEMICOLON(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "R_BRACE"){
        R_BRACE(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::COMMA(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }    
}

void LCRSTree::ASTERISK(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }   
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
}

void LCRSTree::DIVIDE(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }   
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::PLUS(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }   
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "R_BRACKET"){
        R_BRACKET(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::MINUS(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }   
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::MODULO(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }   
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }
}

void LCRSTree::ASSIGNMENT_OPERATOR(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "DOUBLE_QUOTE"){
        DOUBLE_QUOTE(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
}

void LCRSTree::BOOLEAN_EQUAL(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    }
    else if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
}

void LCRSTree::BOOLEAN_AND(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }

}

void LCRSTree::BOOLEAN_NOT(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "IDENTIFIER"){
        identifer(currentToken->next);
    } else if(nextTokenType == "L_PAREN"){
        L_PAREN(currentToken->next);
    }

}

void LCRSTree::INTEGER(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokentype = peek(currentToken);
    if(nextTokentype == "SEMICOLON"){
        SEMICOLON(currentToken->next);
    }
    else if(nextTokentype == "R_PAREN"){
        R_PAREN(currentToken->next);
    }
    else if(nextTokentype == "ASTERISK"){
        ASTERISK(currentToken->next);
    }
    else if(nextTokentype == "DIVIDE"){
        DIVIDE(currentToken->next);
    }
    else if(nextTokentype == "PLUS"){
        PLUS(currentToken->next);
    }
    else if(nextTokentype == "R_BRACKET"){
        R_BRACKET(currentToken->next);
    }
    else if(nextTokentype == "LT"){
        LT(currentToken->next);
    }
    else if(nextTokentype == "GT"){
        GT(currentToken->next);
    }
}

void LCRSTree::DOUBLE_QUOTE(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "STRING"){
        STRING(currentToken->next);
    }
    else if(nextTokenType == "COMMA"){
        COMMA(currentToken->next);
    }
    else if(nextTokenType == "R_PAREN"){
        R_PAREN(currentToken->next);
    }
    else if(nextTokenType == "SEMICOLON"){
        SEMICOLON(currentToken->next);
    }
    else if(nextTokenType == "ESCAPED_CHARACTER"){
        ESCAPED_CHARACTER(currentToken->next);
    }    
}

void LCRSTree::SINGLE_QUOTE(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "STRING"){
        STRING(currentToken->next);
    }
    else if(nextTokenType == "COMMA"){
        COMMA(currentToken->next);
    }
    else if(nextTokenType == "R_PAREN"){
        R_PAREN(currentToken->next);
    } 
    else if(nextTokenType == "SEMICOLON"){
        SEMICOLON(currentToken->next);
    }
    else if(nextTokenType == "PLUS"){
        PLUS(currentToken->next);
    }
    else if(nextTokenType == "ESCAPED_CHARACTER"){
        ESCAPED_CHARACTER(currentToken->next);
    }   
}

void LCRSTree::STRING(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "DOUBLE_QUOTE"){
        DOUBLE_QUOTE(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);//Not the best solution as it allows "I am a string'
    }
    else{
        std::cout << "fail" << std::endl;
        exit(2);
    }
}

void LCRSTree::GT_EQUAL(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
}

void LCRSTree::GT(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
}

void LCRSTree::LT_EQUAL(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }    
}

void LCRSTree::LT(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "INTEGER"){
        INTEGER(currentToken->next);
    }
    else if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }    
}

void LCRSTree::ESCAPED_CHARACTER(Node *currentToken)
{
    addRightSibling(currentToken->theToken, currentToken->lineNumber + 1, currentToken->tokenType);
    std::string nextTokenType = peek(currentToken);
    if(nextTokenType == "SINGLE_QUOTE"){
        SINGLE_QUOTE(currentToken->next);
    }
    else{
    std::cout << "Syntax error on line " << currentToken->lineNumber + 1 << ": unterminated string quote." << std::endl;
    exit(2); 
    }
}

std::string LCRSTree::reservedWordChecker(std::string wordToCheck)
{
    if(wordToCheck == "char"){
        return "char";
    }
    else if(wordToCheck == "void"){
        return "void";
    }
    else if(wordToCheck == "int"){
        return "int";
    }
    else if(wordToCheck == "printf"){
        return "printF";
    }
    return "";
}

std::vector<treeNode*> LCRSTree::booleanExpressionPostFix(const std::vector<treeNode*>& treeNodeList)
{
    std::stack<treeNode*> st;
    std::vector<treeNode*> res;
    bool finished;
    treeNode* temp = nullptr;
    for(treeNode* walker : treeNodeList){
        if ((walker->tokenType == "INTEGER") or (walker->tokenType == "IDENTIFIER") or (walker->tokenType == "SINGLE_QUOTE") or (walker->tokenType == "DOUBLE_QUOTE") or (walker->tokenType == "STRING") or (walker->tokenType == "L_BRACKET") or (walker->tokenType == "R_BRACKET")){
            res.push_back(walker);
        }
        else{
            if(walker->tokenType == "L_PAREN"){
                st.push(walker);
            }
            else{
                if(walker->tokenType == "R_PAREN"){
                    finished = false;
                    while(!finished){
                        if(st.top()->tokenType == "L_PAREN"){
                            st.pop();
                            finished = true;
                        }
                        else{
                            res.push_back(st.top());
                            st.pop();
                        }
                    }
                }
                else{
                    if((walker->tokenType == "BOOLEAN_EQUAL") or (walker->tokenType == "BOOLEAN_NOT") or (walker->tokenType == "LT") or (walker->tokenType == "GT") or (walker->tokenType == "LT_EQUAL") or (walker->tokenType == "GT_EQUAL") or (walker->tokenType == "BOOLEAN_AND") or (walker->tokenType == "BOOLEAN_OR") or (walker->tokenType == "BOOLEAN_NOT") or (walker->tokenType == "PLUS") or (walker->tokenType == "MINUS") or (walker->tokenType == "ASTERISK") or (walker->tokenType == "DIVIDE") or (walker->tokenType == "MODULO")){
                        if(st.empty()){
                            st.push(walker);
                        }
                        else{
                            if(st.top()->tokenType == "L_PAREN"){
                                st.push(walker);
                            }
                            else{
                                if(walker->tokenType == "BOOLEAN_Not"){
                                    finished = false;
                                    while(!finished){
                                        if(st.empty()){
                                            if(st.top()->tokenType == "BOOLEAN_NOT"){
                                                res.push_back(st.top());
                                                st.pop();
                                            }
                                            else{
                                                st.push(walker);
                                                finished = true;
                                            }
                                        }
                                        else{
                                            st.push(walker);
                                            finished = true;
                                        }
                                    }
                                }
                                else{
                                    if((walker->tokenType == "ASTERISK") or (walker->tokenType == "DIVIDE") or (walker->tokenType == "MODULO")){
                                        finished = false;
                                        while(!finished){
                                            if(st.empty()){
                                                if((st.top()->tokenType == "BOOLEAN_NOT") or (st.top()->tokenType == "ASTERISK") or (st.top()->tokenType == "DIVIDE") or (st.top()->tokenType == "MODULO")){
                                                    res.push_back(st.top());
                                                    st.pop();
                                                }
                                                else{
                                                    st.push(walker);
                                                    finished = true;
                                                }
                                            }
                                            else{
                                                st.push(walker);
                                                finished = true;
                                            }
                                        }
                                    }
                                    else{
                                        if ((walker->tokenType == "PLUS") or walker->tokenType == "MINUS"){
                                            finished = false;
                                            while(!finished){
                                                if(st.empty()){
                                                    if ((st.top()->tokenType == "BOOLEAN_NOT") or (st.top()->tokenType == "ASTERISK") or (st.top()->tokenType == "DIVIDE") or (st.top()->tokenType == "MODULO") or (st.top()->tokenType == "PLUS") or (st.top()->tokenType == "MINUS")){
                                                        res.push_back(st.top());
                                                        st.pop();
                                                    }
                                                    else{
                                                        st.push(walker);
                                                        finished = true;
                                                    }
                                                }
                                                else{
                                                    st.push(walker);
                                                    finished = true;
                                                }
                                            }
                                        }
                                        else{
                                            if ((walker->tokenType == "BOOLEAN_EQUAL") or (walker->tokenType == "NOT_EQUAL") or (walker->tokenType == "LT") or (walker->tokenType == "GT") or (walker->tokenType == "LT_EQUAL") or (walker->tokenType == "GT_EQUAL")){
                                                finished = false;
                                                while(!finished){
                                                    if(st.empty()){
                                                        if ((st.top()->tokenType == "BOOLEAN_NOT") || (st.top()->tokenType == "ASTERISK") ||(st.top()->tokenType == "DIVIDE") || (st.top()->tokenType == "MODULO") ||(st.top()->tokenType == "PLUS") || (st.top()->tokenType == "MINUS") ||(st.top()->tokenType == "BOOLEAN_EQUAL") || (st.top()->tokenType == "NOT_EQUAL") ||(st.top()->tokenType == "LT") || (st.top()->tokenType == "GT") ||(st.top()->tokenType == "LT_EQUAL") || (st.top()->tokenType == "GT_EQUAL") ||(st.top()->tokenType == "BOOLEAN_NOT")){
                                                            res.push_back(st.top());
                                                            st.pop();
                                                        }
                                                        else{
                                                            st.push(walker);
                                                            finished = true;
                                                        }
                                                    }
                                                    else{
                                                        st.push(walker);
                                                        finished = true;
                                                    }
                                                }
                                            }
                                            else{
                                                if(walker->tokenType == "BOOLEAN_AND"){
                                                    finished = false;
                                                    while(!finished){
                                                        if(st.empty()){
                                                            if ((st.top()->tokenType == "BOOLEAN_NOT") || (st.top()->tokenType == "ASTERISK") ||(st.top()->tokenType == "DIVIDE") || (st.top()->tokenType == "MODULO") ||(st.top()->tokenType == "PLUS") || (st.top()->tokenType == "MINUS") ||(st.top()->tokenType == "BOOLEAN_EQUAL") || (st.top()->tokenType == "NOT_EQUAL") ||(st.top()->tokenType == "LT") || (st.top()->tokenType == "GT") ||(st.top()->tokenType == "LT_EQUAL") || (st.top()->tokenType == "GT_EQUAL") ||(st.top()->tokenType == "BOOLEAN_AND") || (st.top()->tokenType == "BOOLEAN_NOT")){
                                                                res.push_back(st.top());
                                                                st.pop();
                                                            }
                                                            else{
                                                                st.push(walker);
                                                                finished = true;
                                                            }
                                                        }
                                                        else{
                                                            st.push(walker);
                                                            finished = true;
                                                        }
                                                    }
                                                }
                                                else{
                                                    if(walker->tokenType == "BOOLEAN_OR"){
                                                        finished = false;
                                                        while(!finished){
                                                            if(st.empty()){
                                                                if ((st.top()->tokenType == "BOOLEAN_NOT") ||(st.top()->tokenType == "ASTERISK") ||(st.top()->tokenType == "DIVIDE") ||(st.top()->tokenType == "MODULO") ||(st.top()->tokenType == "PLUS") ||(st.top()->tokenType == "MINUS") ||(st.top()->tokenType == "BOOLEAN_EQUAL") ||(st.top()->tokenType == "NOT_EQUAL") ||(st.top()->tokenType == "LT") ||(st.top()->tokenType == "GT") ||(st.top()->tokenType == "LT_EQUAL") ||(st.top()->tokenType == "GT_EQUAL") ||(st.top()->tokenType == "BOOLEAN_AND") ||(st.top()->tokenType == "BOOLEAN_OR") ||(st.top()->tokenType == "BOOLEAN_NOT")){
                                                                    res.push_back(st.top());
                                                                    st.pop();
                                                                }
                                                                else{
                                                                    st.push(walker);
                                                                    finished = true;
                                                                }
                                                            }
                                                            else{
                                                                st.push(walker);
                                                                finished = true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    while(!st.empty()){
        res.push_back(st.top());
        st.pop();
    }
    return res;
}

void LCRSTree::addRightSibling(std::string lineToAdd, int currentLine, std::string tokenToAdd)
{
    treeNode* newNode = new treeNode;
    newNode->line = lineToAdd;
    newNode->lineNumber = currentLine;
    newNode->tokenType = tokenToAdd;
    if(root == nullptr){
        root = newNode;
        last = newNode;
        return;
    }
    last->next = newNode;
    last = newNode;
}

void LCRSTree::addLeftChild(std::string lineToAdd, int currentLine, std::string tokenToAdd)
{
    if(last == nullptr){
        std::cout << "ERROR: Attempted to add leftChild to nonexistent parent" << std::endl;
        exit(2);
    }
    treeNode* newNode = new treeNode;
    newNode->line = lineToAdd;
    newNode->lineNumber = currentLine;
    newNode->tokenType = tokenToAdd;
    last->leftChild = newNode;
    last = newNode;
}

std::string LCRSTree::peek(Node *walker)
{
    std::string nextToken = "";
    if(walker->next == nullptr){
        return nextToken;
    }
    nextToken = walker->next->tokenType;
    return nextToken;
}

std::ostream &operator<<(std::ostream &out, const LCRSTree& tree)
{
    treeNode* walker = tree.root;
    while(walker != nullptr){
        pStart:
        out << walker->line << "    ";

        if(walker->next == nullptr){
            walker = walker->leftChild;
            out << std::endl;
            if(walker == nullptr){//There is nothing more
                return out;
            }
            goto pStart;
        }
        walker = walker->next;
    }
    return out;
}