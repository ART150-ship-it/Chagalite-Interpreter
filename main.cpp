#include <iostream>
#include <fstream>
#include "Interpreter.hpp"


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
    // std::cout << CST;

    SymbolTable ST(CST);
    // std::cout << ST;

    AST ast(CST, ST);

    // ast.print(); std::cout << std::endl;
    // std::cout << "INTERPRETED RESULT:\n";

    Interpreter interpreter(&ST);

    return 0;
}


