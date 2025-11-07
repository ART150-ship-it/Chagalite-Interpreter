#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include "linkedlist.hpp"

linklist::linklist()
{
    //unused
}

linklist::linklist(std::string fileName){
    std::ifstream inFS;
    inFS.open(fileName);
    if(!inFS.is_open()){
        std::cout << "Could not open file" << std::endl;
        exit(2);
    }
    bool isComment = false;
    int lineCounter = 0;
    std::string stringOfWords;
    while(getline(inFS,stringOfWords)){
        lineCounter++;
        int startofComment;
        int endofComment = 0;
        int startofLine = 0;
        for(int i = 0; i < stringOfWords.length(); i++){
            start:
            if(stringOfWords[i] == '"'){
                goto skip;
            }
            if(stringOfWords[i] == '*'){
                if(stringOfWords[i + 1] == '/'){
                    std::cout << "Error: Program contains C-style, unterminated comment on line " << lineCounter << "." << std::endl;
                    exit(2);
                }
            }
            if(stringOfWords[i] == '/'){
                if(stringOfWords[i + 1] == '/'){
                    std::fill(stringOfWords.begin() + i,stringOfWords.end(), ' ');
                    goto skip;
                }
                if(stringOfWords[i + 1] == '*'){
                    startofLine = lineCounter;
                    startofComment = i;
                    isComment = true;
                    cstart:
                    for(int z = i + 1; z < stringOfWords.length(); z++){
                        if(stringOfWords[z] == '*'){
                            if(stringOfWords[z + 1] == '/'){
                                endofComment = z + 1;
                                std::fill(stringOfWords.begin() + startofComment, stringOfWords.begin() + endofComment + 1, ' ');
                                startofComment = 0;
                                endofComment = 0;
                                i = z;
                                isComment = false;
                                goto start;
                            }
                        }
                    }
                    std::fill(stringOfWords.begin() + startofComment, stringOfWords.end(), ' ');
                    Node* newNode = new Node;
                    newNode->line = stringOfWords;
                    newNode->lineNumber = lineCounter;//new line
                    InsertNodeLast(newNode);
                    if(getline(inFS,stringOfWords)){
                        lineCounter++;
                        startofComment = 0;
                        endofComment = 0;
                        i = -1;
                        goto cstart;
                    }
                    else{
                        std::cout << "Error: Programs contains C-style, unterminated comment on line " << startofLine << "." << std::endl;
                        exit(2);
                    }
                }
            }
        }
        skip:
        if(isComment){
            std::fill(stringOfWords.begin(), stringOfWords.end(),' ');
        }
        Node* newNode = new Node;
        newNode->line = stringOfWords;
        newNode->lineNumber = lineCounter; //new line
        InsertNodeLast(newNode);
    }
}

linklist::linklist(const linklist& other){
    Node* walker = other.first;
    Node* token = nullptr;
    int lineCounter = 0;
    bool validQUOTES = false;
    std::string currentLine = "";
    std::string currentToken = "";
    while(walker != nullptr){
        currentLine = walker->line;
        for(auto i = 0; i < currentLine.length(); i++){
            if(!std::isspace(currentLine[i])){
                for(;i < currentLine.length(); i++){
                    if(std::isspace(currentLine[i])){
                        if(!currentToken.empty()){
                            token = tokenIdentifer(currentToken, lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                    }
                    else if(currentLine[i] == '('){
                        if(currentToken != ""){ //Figure out what the token is
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = "(";
                        token->tokenType = "L_PAREN";
                        InsertNodeLast(token);
                    }
                    else if(currentLine[i] == ')'){
                        if(currentToken != ""){ //Figure out what the token is
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = ")";
                        token->tokenType = "R_PAREN";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == '['){
                        if(currentToken != ""){ //Figure out what the token is
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = "[";
                        token->tokenType = "L_BRACKET";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                    }
                    else if(currentLine[i] == ']'){
                        if(currentToken != ""){ //Figure out what the token is
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = "]";
                        token->tokenType = "R_BRACKET";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == '{'){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = "{";
                        token->tokenType = "L_BRACE";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == '}'){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = "}";
                        token->tokenType = "R_BRACE";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == ';'){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = ';';
                        token->tokenType = "SEMICOLON";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == ','){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = ',';
                        token->tokenType = "COMMA";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        currentToken = "";
                    }
                    else if(currentLine[i] == '"'){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = '"';
                        token->tokenType = "DOUBLE_QUOTE";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        i++;
                        for(; i < currentLine.length(); i++){
                            if(currentLine[i] == '"'){
                                token = tokenIdentifer(currentToken,lineCounter);
                                token->tokenType = "STRING";
                                token->lineNumber = lineCounter;// new Line
                                InsertNodeLast(token);
                                token = new Node;
                                token->theToken = '"';
                                token->tokenType = "DOUBLE_QUOTE";
                                token->lineNumber = lineCounter;// new Line
                                InsertNodeLast(token);
                                currentToken = "";
                                validQUOTES = true;
                                goto DoubleQuoteSkip;
                            }
                            else{
                                currentToken.push_back(currentLine[i]);
                            }
                            if(currentLine[i + 1] == '\\'){
                                currentToken.push_back(currentLine[i+1]);
                                currentToken.push_back(currentLine[i+2]);
                                i = i + 2;
                            }
                        }
                        DoubleQuoteSkip:
                        // if(!validQUOTES){
                        //     std::cout << "Syntax error on line " << lineCounter + 1 << ": missing closing double quote" << std::endl;
                        //     exit(2);
                        // }
                        validQUOTES = false;
                    }
                    else if(currentLine[i] == '\''){
                        if(currentToken != ""){
                            token = tokenIdentifer(currentToken,lineCounter);
                            token->lineNumber = lineCounter;// new Line
                            InsertNodeLast(token);
                            currentToken = "";
                        }
                        token = new Node;
                        token->theToken = '\'';
                        token->tokenType = "SINGLE_QUOTE";
                        token->lineNumber = lineCounter;// new Line
                        InsertNodeLast(token);
                        i++;
                        for(; i < currentLine.length(); i++){
                            if(currentLine[i] == '\''){
                                token = tokenIdentifer(currentToken,lineCounter);
                                if(token->tokenType != "ESCAPED_CHARACTER"){
                                    token->tokenType = "STRING";
                                }
                                //token->tokenType = "STRING";
                                token->lineNumber = lineCounter;// new Line
                                InsertNodeLast(token);
                                token = new Node;
                                token->theToken = '\'';
                                token->tokenType = "SINGLE_QUOTE";
                                token->lineNumber = lineCounter;// new Line
                                InsertNodeLast(token);
                                currentToken = "";
                                validQUOTES = true;
                                goto shortQuoteSkip;
                            }
                            else{
                                currentToken.push_back(currentLine[i]);
                            }
                        }
                        shortQuoteSkip:
                        if(!validQUOTES){
                            std::cout << "Syntax error on line " << lineCounter + 1 << ": missing closing single quote" << std::endl;
                            exit(2);
                        }
                        validQUOTES = false;
                    }
                    else{
                        currentToken.push_back(currentLine[i]); //Builds the token
                    }
                }
            }
            if(currentToken != ""){ //Figure out what the token is
                token = tokenIdentifer(currentToken, lineCounter);
                token->lineNumber = lineCounter;// new Line
                InsertNodeLast(token);
                currentToken = "";
            }

        }

        walker = walker->next;
        lineCounter++;
    }
}

bool linklist::InsertNodeLast(Node* newNode){
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

Node* linklist::tokenIdentifer(std::string line, int lineCounter)
{
    Node* token = new Node;
    token->theToken = line;
    for(int i = 0; i < line.length(); i++){
        if(std::isdigit(line[i])){
            for(; i < line.length(); i++){
                if(!std::isdigit(line[i])){
                    std::cout << "Syntax error on line " << lineCounter + 1 << ": invalid integer" << std::endl;
                    exit(2);
                }
            }
            token->tokenType = "INTEGER";
            return token;
        }
        else if(std::isalpha(line[i])){
            token->tokenType = "IDENTIFIER";
            return token;
        }
        else if(line[i] == '='){
            if(line[i + 1] == '='){
                token->tokenType = "BOOLEAN_EQUAL";
                return token;
            }
            token->tokenType = "ASSIGNMENT_OPERATOR";
            return token;
        }
        else if(line[i] == '%'){
            token->tokenType = "MODULO";
            return token;
        }
        else if(line[i] == '*'){
            token->tokenType = "ASTERISK";
            return token;
        }
        else if(line[i] == '/'){
            token->tokenType = "DIVIDE";
            return token;
        }
        else if(line[i] == '+'){
            // if(std::isdigit(line[i + 1])){
            //     token->tokenType = "INTEGER";
            //     return token;
            // }            
            token->tokenType = "PLUS";
            return token;
        }
        else if(line[i] == '-'){
            if(std::isdigit(line[i + 1])){
                token->tokenType = "INTEGER";
                return token;
            }
            token->tokenType = "MINUS";
            return token;
        }
        else if(line[i] == '<'){
            if(line[i + 1] == '='){
                token->tokenType = "LT_EQUAL";
                return token;
            }
            token->tokenType = "LT";
            return token;
        }
        else if(line[i] == '>'){
            if(line[i + 1] == '='){
                token->tokenType = "GT_EQUAL";
                return token;
            }
            token->tokenType = "GT";
            return token;
        }
        else if(line[i] == '&'){
            if(line[i + 1] == '&'){
                token->tokenType = "BOOLEAN_AND";
                return token;
            }
            token->tokenType = "UNKNOWN";
            return token;
        }
        else if(line[i] == '!'){
            if(line[i + 1] == '='){
                token->tokenType = "BOOLEAN_NOT_EQUAL";
                return token;
            }
            if(isalpha(line[i + 1])){
                std::string tempString = token->theToken;
                char singledOut = tempString.front();
                std::string restOfString = tempString.substr(1);
                token->theToken = singledOut;
                token->tokenType = "BOOLEAN_NOT";
                token->lineNumber = lineCounter;// new Line
                InsertNodeLast(token);
                token = new Node;
                token->theToken = restOfString;
                token->tokenType = "IDENTIFIER";
                return token;
            }
            token->tokenType = "BOOLEAN_NOT";
            return token;
        }
        else if(line[i] == '\\'){
            if(line[i + 1] == 'x'){
                if(hexChecker(line[i + 2])){
                    if(hexChecker(line[i + 3])){
                        token->tokenType = "ESCAPED_CHARACTER";
                        return token;
                    }
                    token->tokenType = "ESCAPED_CHARACTER";
                    return token;
                }
            }
            if(escapedCharacterChecker(line[i + 1])){
                token->tokenType = "ESCAPED_CHARACTER";
                return token;
            }
        }
    }
    token->tokenType = "UNKNOWN";
    return token;
}

bool linklist::hexChecker(char line){
    std::vector<char> hexValues = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','C','E','F','a','b','c','d','e','f'};

    for(char curChar : hexValues){
        if(line == curChar){
            return true;
        }
    }
    return false;
}

bool linklist::escapedCharacterChecker(char line){
    std::vector<char> escapedCharacters = {'a','b','f','n','r','t','v','\\','?','\'','\"'};
    for(char curChar : escapedCharacters){
        if(line == curChar){
            return true;
        }
    }
    return false;    
}

std::ostream &operator<<(std::ostream &out, const linklist &LL){
    out << "Token list:" << std::endl;
    out << std::endl;
    Node* walker = LL.first;
    while(walker != nullptr){
        out << "Token type: " << walker->tokenType << std::endl;
        out << "Token:      " << walker->theToken << std::endl;
        out << std::endl;
        walker = walker->next;
    }
    // Node* walker = LL.first;
    // while(walker != nullptr){
    //     out << walker->line << std::endl;
    //     walker = walker->next;
    // }
    return out;
}