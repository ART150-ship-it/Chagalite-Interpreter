#pragma once
#include <ostream>
#include <vector>
#include <stack>
#include "linkedlist.hpp"

struct treeNode {
    std::string line;
    std::string tokenType;
    int lineNumber = -1;
    treeNode* next = nullptr;
    treeNode* leftChild = nullptr;
};

class LCRSTree {
    public:
        LCRSTree();
        LCRSTree(const linklist& other);
        LCRSTree(const LCRSTree& otherTree);
        void addRightSibling(std::string lineToAdd, int currentLine, std::string token);
        void addLeftChild(std::string lineToAdd, int currentLine, std::string token);
        std::string peek(Node* walker);

        void DOUBLE_QUOTE(Node* currentToken);
        void SINGLE_QUOTE(Node* currentToken);
        void STRING(Node* currentToken);
        void identifer(Node* currentToken);
        void L_PAREN(Node* currentToken);
        void R_PAREN(Node* currentToken);    
        void L_BRACE(Node* currentToken);
        void R_BRACE(Node* currentToken);   
        void L_BRACKET(Node* currentToken);
        void R_BRACKET(Node* currentToken); 
        void SEMICOLON(Node* currentToken);
        void COMMA(Node* currentToken);
        void ASTERISK(Node* currentToken);
        void DIVIDE(Node* currentToken);
        void PLUS(Node* currentToken);
        void MINUS(Node* currentToken);
        void MODULO(Node* currentToken);
        void ASSIGNMENT_OPERATOR(Node* currentToken);
        void BOOLEAN_EQUAL(Node* currentToken);
        void BOOLEAN_AND(Node* currentToken);
        void BOOLEAN_NOT(Node* currentToken);
        void INTEGER(Node* currentToken);
        void GT_EQUAL(Node* currentToken);
        void GT(Node* currentToken);
        void LT_EQUAL(Node* currentToken);
        void LT(Node* currentToken);
        void ESCAPED_CHARACTER(Node* currentToken);
        static std::vector<treeNode*> booleanExpressionPostFix(const std::vector<treeNode*>& treeNodeList);
        
        std::string reservedWordChecker(std::string wordToCheck);
        treeNode* getRoot() const {return root;}

        friend std::ostream& operator<< (std::ostream& out, const LCRSTree& tree);
    private:
        treeNode* root = nullptr;
        treeNode* last = nullptr;
        int lineCounter = 0;
        bool isForLoop = false;
};