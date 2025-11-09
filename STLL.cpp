#include <iostream>
#include "STLL.hpp"

SymbolTable::SymbolTable(const LCRSTree &other)
{
    int scope = 0;
    bool isGlobal = true;
    treeNode* walker = other.getRoot();
    while(walker != nullptr){
        pStart:
        if(walker->line == "function"){
            scope++;
            walker = isFunction(walker,scope);
        }
        else if(walker->line == "int"){
            if(isGlobal){
                walker = isInt(walker,0);
            }
            else{
                walker = isInt(walker,scope);
            }
        }
        else if(walker->line == "bool"){
            if(isGlobal){
                walker = isBool(walker,0);
            }
            else{
                walker = isBool(walker,scope);
            }
        }
        else if(walker->line == "char"){
            if(isGlobal){
                walker = isChar(walker,0);
            }
            else{
                walker = isChar(walker,scope);
            }
        }
        else if(walker->line == "procedure"){
            scope++;
            walker = isProcedure(walker,scope);
        }
        if(walker->line == "{"){
            isGlobal = false;
        }
        if(walker->line == "}"){
            isGlobal = true;
        }

        if(walker->next == nullptr){
            walker = walker->leftChild;
            if(walker == nullptr){//There is nothing more
                return;
            }
            goto pStart;
        }
        walker = walker->next;
    }
}

void SymbolTable::alreadyDefined(std::string name, int scopeNumber, int currentLineNumber)
{
    STNode* walker = first;
    if(walker == nullptr){
        return;
    }
    if(scopeNumber == 0){//We have a global need to check if it exists anywhere
        while(walker != nullptr){
            if(walker->identifierName == name){
                std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined global" << std::endl;
                exit(2);
            }
            walker = walker->next;
        }
        walker = paramFirst;
        while(walker != nullptr){
            if(walker->identifierName == name){
                std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined global" << std::endl;
                exit(2);
            }
            walker = walker->next;
        }
        return;
    }

    //Have a local that needs to be checked if its a global, or in the local area
    while(walker != nullptr){
        if(name == walker->identifierName and walker->scope == 0){//already defined as global
            std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined globally" << std::endl;
            exit(2);
        }
        else if(name == walker->identifierName and scopeNumber == walker->scope){//already defined in local space
            std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined locally" << std::endl;
            exit(2);
        }
        walker = walker->next;
    }
    walker = paramFirst;
    while(walker != nullptr){
        if(name == walker->identifierName and scopeNumber == 0){//already defined as global
            std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined globally" << std::endl;
            exit(2);
        }
        else if(name == walker->identifierName and scopeNumber == walker->scope){//already defined in local space
            std::cout << "Error on line " << currentLineNumber << ": variable \"" << name << "\" is already defined locally" << std::endl;
            exit(2);
        }
        walker = walker->next;
    }
}

treeNode* SymbolTable::isFunction(treeNode *node, int currentScope)
{
    bool isFirst = true;
    STNode* newNode = new STNode;
    newNode->identifierType = node->line;
    node = node->next;
    newNode->dataType = node->line;
    node = node->next;
    alreadyDefined(node->line,currentScope, node->lineNumber);
    newNode->identifierName = node->line;
    std::string functionName = node->line;
    newNode->datatypeIsArray = "no";
    newNode->datatypeArraySize = 0;
    newNode->scope = currentScope;
    InsertNodeLast(newNode);
    while(node->line != ")"){
        if(node->line == "int"){
            newNode = new STNode;
            if(isFirst){
                newNode->parameterListFor = functionName;
                isFirst = false;
            }
            newNode->dataType = node->line;
            node = node->next;
            alreadyDefined(node->line,currentScope, node->lineNumber);
            newNode->identifierName = node->line;
            newNode->datatypeIsArray = "no";
            newNode->datatypeArraySize = 0;
            newNode->scope = currentScope;
            InsertNodeParamLast(newNode);
        }
        else if(node->line == "char"){
            newNode = new STNode;
            if(isFirst){
                newNode->parameterListFor = functionName;
                isFirst = false;
            }
            newNode->dataType = node->line;
            node = node->next;
            alreadyDefined(node->line,currentScope, node->lineNumber);
            newNode->identifierName = node->line;
            if(node->next->line == "["){
                node = node->next;
                newNode->datatypeIsArray = "yes";
                node = node->next;
                newNode->datatypeArraySize = std::stoi(node->line);
            }
            else{
                newNode->datatypeIsArray = "no";
                newNode->datatypeArraySize = 0;
            }
            newNode->scope = currentScope;
            InsertNodeParamLast(newNode);            
        }
        else if(node->line == "bool"){
            newNode = new STNode;
            if(isFirst){
                newNode->parameterListFor = functionName;
                isFirst = false;
            }
            newNode->dataType = node->line;
            node = node->next;
            alreadyDefined(node->line,currentScope, node->lineNumber);
            newNode->identifierName = node->line;
            newNode->datatypeIsArray = "no";
            newNode->datatypeArraySize = 0;
            newNode->scope = currentScope;
            InsertNodeParamLast(newNode);
        }
        node = node->next;
    }
    return node;
}

treeNode *SymbolTable::isInt(treeNode *node, int currentScope)
{
    STNode* newNode = new STNode;
    newNode->dataType = node->line;
    node = node->next;
    alreadyDefined(node->line,currentScope, node->lineNumber);
    newNode->identifierName = node->line;
    newNode->identifierType = "datatype";
    newNode->datatypeIsArray = "no";
    newNode->datatypeArraySize = 0;
    newNode->scope = currentScope;
    InsertNodeLast(newNode);
    while(node->next->line == ","){
        newNode = new STNode;
        newNode->dataType = "int";
        node = node->next->next;
        alreadyDefined(node->line,currentScope, node->lineNumber);
        newNode->identifierName = node->line;
        newNode->identifierType = "datatype";
        newNode->datatypeIsArray = "no";
        newNode->datatypeArraySize = 0;
        newNode->scope = currentScope;
        InsertNodeLast(newNode);
    }
    return node->next;
}

treeNode *SymbolTable::isChar(treeNode *node, int currentScope)
{
    STNode* newNode = new STNode;
    newNode->dataType = node->line;
    node = node->next;
    alreadyDefined(node->line,currentScope, node->lineNumber);
    newNode->identifierName = node->line;
    newNode->identifierType = "datatype";
    if(node->next->line == "["){
        node = node->next;
        newNode->datatypeIsArray = "yes";
        node = node->next;
        newNode->datatypeArraySize = std::stoi(node->line);
    }
    else{
        newNode->datatypeIsArray = "no";
        newNode->datatypeArraySize = 0;
    }
    newNode->scope = currentScope;
    InsertNodeLast(newNode);
    return node->next;
}

treeNode *SymbolTable::isBool(treeNode *node, int currentScope)
{
    STNode* newNode = new STNode;
    newNode->dataType = node->line;
    node = node->next;
    alreadyDefined(node->line,currentScope, node->lineNumber);
    newNode->identifierName = node->line;
    newNode->identifierType = "datatype";
    newNode->datatypeIsArray = "no";
    newNode->datatypeArraySize = 0;
    newNode->scope = currentScope;
    InsertNodeLast(newNode);
    return node->next;
}

treeNode *SymbolTable::isProcedure(treeNode *node, int currentScope)
{
    bool isFirst = true;
    STNode* newNode = new STNode;
    newNode->identifierType = node->line;
    node = node->next;
    alreadyDefined(node->line,currentScope, node->lineNumber);
    newNode->identifierName = node->line;
    std::string functionName = node->line;
    newNode->dataType = "NOT APPLICABLE";
    newNode->datatypeIsArray = "no";
    newNode->datatypeArraySize = 0;
    newNode->scope = currentScope;
    InsertNodeLast(newNode);
    while(node->line != ")"){
        if(node->line == "int"){
            newNode = new STNode;
            if(isFirst){
                newNode->parameterListFor = functionName;
                isFirst = false;
            }
            newNode->dataType = node->line;
            node = node->next;
            alreadyDefined(node->line,currentScope, node->lineNumber);
            newNode->identifierName = node->line;
            newNode->datatypeIsArray = "no";
            newNode->datatypeArraySize = 0;
            newNode->scope = currentScope;
            InsertNodeParamLast(newNode);
        }
        else if(node->line == "char"){
            newNode = new STNode;
            if(isFirst){
                newNode->parameterListFor = functionName;
                isFirst = false;
            }
            newNode->dataType = node->line;
            node = node->next;
            alreadyDefined(node->line,currentScope, node->lineNumber);
            newNode->identifierName = node->line;
            if(node->next->line == "["){
                node = node->next;
                newNode->datatypeIsArray = "yes";
                node = node->next;
                newNode->datatypeArraySize = std::stoi(node->line);
            }
            else{
                newNode->datatypeIsArray = "no";
                newNode->datatypeArraySize = 0;
            }
            newNode->scope = currentScope;
            InsertNodeParamLast(newNode);           
        }
        node = node->next;
    }
    return node;
}


bool SymbolTable::InsertNodeLast(STNode *newNode)
{
    if(newNode == nullptr){
        return false;
    }
    if(size == 0){
        first = newNode;
        last = newNode;
        size++;
        return true;
    }
    last->next = newNode;
    last = newNode;
    size++;
    return true;
}

bool SymbolTable::InsertNodeParamLast(STNode *newNode)
{
    if(newNode == nullptr){
        return false;
    }
    if(paramSize == 0){
        paramFirst = newNode;
        paramLast = newNode;
        paramSize++;
        return true;
    }
    paramLast->next = newNode;
    paramLast = newNode;
    paramSize++;
    return true;
}

STNode* SymbolTable::resolve(std::string ident, int scope) const
{
    STNode* walker = first;

    while (walker) {
        if (walker->identifierName == ident && (walker->scope == scope || walker->scope == 0 || walker->identifierType == "function" || walker->identifierType == "procedure")) {
            return walker;
        }
        walker = walker->next;
    }

    walker = paramFirst;
    while (walker) {
        if (walker->identifierName == ident && (walker->scope == scope || walker->scope == 0)) {
            return walker;
        }
        walker = walker->next;
    }

    return nullptr;
}

std::ostream &operator<<(std::ostream &out, const SymbolTable &LL)
{
    STNode* walker = LL.first;
    while (walker != nullptr){
        out << "IDENTIFIER_NAME: " << walker->identifierName << std::endl;
        out << "IDENTIFIER_TYPE: " << walker->identifierType << std::endl;
        out << "DATATYPE: " << walker->dataType << std::endl;
        out << "DATATYPE_IS_ARRAY: " << walker->datatypeIsArray << std::endl;
        out << "DATATYPE_ARRAY_SIZE: " << walker->datatypeArraySize << std::endl;
        out << "SCOPE: " << walker->scope << std::endl;
        out << std::endl;
        walker = walker->next;
    }
    walker = LL.paramFirst;
    out << std::endl;
    while (walker != nullptr){
        if(walker->parameterListFor != ""){
            out << "PARAMETER LIST FOR : " << walker->parameterListFor << std::endl;
        }
        out << "IDENTIFIER_NAME: " << walker->identifierName << std::endl;
        out << "DATATYPE: " << walker->dataType << std::endl;
        out << "DATATYPE_IS_ARRAY: " << walker->datatypeIsArray << std::endl;
        out << "DATATYPE_ARRAY_SIZE: " << walker->datatypeArraySize << std::endl;
        out << "SCOPE: " << walker->scope << std::endl;
        out << std::endl;
        walker = walker->next;
    }
    return out;
}