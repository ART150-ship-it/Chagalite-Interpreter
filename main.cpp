#include <iostream>
#include <fstream>
#include "linkedlist.hpp"
#include "LCRS.hpp"
#include "STLL.hpp"

int main(int argc, char *argv[]){
    if(argc != 2){
        std::cout << "usage: " << argv[0] << " inputFileNameThatContainsProgram\n";
        exit(1);
    }

    linklist removedComments(argv[1]);
    //std::cout << removedComments;

    linklist tokenizer(removedComments);
    //std::cout << tokenizer;
    // function int sum_of_first_n_squares (int n)

    LCRSTree CST(tokenizer);
    //std::cout << CST;

    SymbolTable ST(CST);
    //std::cout << ST;

    //Take a LCRS from the CST and go through it. When function, procedure, or a reserved character (like int) is encountered put down DECLARATION and skip until left child
    //If a "=" is encountered during a peek() then put down ASSIGNMENT then start collecting everything from that like into a vector until a ";" is found. Then give vector to
    //postfix function to add the things to where they need to go. If you see a "if" or a "while" add what it is (like if its an "if" or "while") then add everything after into a vector
    //to send to a postfix function for booleans. Stop when a "{" is found. For a "For" loop it would similar, but it needs to be a three parter. It would have FOR EXXPRESSION 1 and so on
    //Simply for the first 2 go until a ";" is found while also building and sending the vector to the function each time a semicolon is seen then look for a ")" then send that to the postfix funtion. 

    LCRSTree AST(CST);
    std::cout << AST;

    return 0;
}


