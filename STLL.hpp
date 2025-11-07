#include <string>
#include "LCRS.hpp"

struct STNode {
    std::string identiferName = "";
    std::string identiferType = "";
    std::string dataType = "";
    std::string datatypeIsArray = "";
    int datatypeArraySize = -1;
    int scope = -1;
    std::string parameterListFor = "";
    STNode* next = nullptr;
    int lineNumber = 0;
};


class SymbolTable {
    public:
    SymbolTable();
    SymbolTable(const LCRSTree& other);

    bool InsertNodeLast(STNode *newNode); 
    bool InsertNodeParamLast(STNode *newNode);
    void alreadyDefined(std::string name, int scopeNumber, int currentLineNumber);
    treeNode* isFunction(treeNode *node, int currentScope);
    treeNode* isInt(treeNode *node, int currentScope);
    treeNode* isChar(treeNode *node, int currentScope);
    treeNode* isBool(treeNode *node, int currentScope);
    treeNode* isProcedure(treeNode *node, int currentScope);
    friend std::ostream& operator<< (std::ostream& out, const SymbolTable& LL);



    private:
    STNode* first = nullptr;
    STNode* last = nullptr;
    STNode* paramFirst = nullptr;
    STNode* paramLast = nullptr;
    int size = 0;
    int paramSize = 0;
};