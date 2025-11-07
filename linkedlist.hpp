#pragma once
#include <ostream>
struct Node {
    std::string line;
    std::string tokenType;
    std::string theToken;
    Node* next = nullptr;
    int lineNumber = 0;
};

class linklist {
    public:
    linklist();
    linklist(std::string fileName);
    linklist(const linklist& other);

    bool InsertNodeLast(Node *newNode);
    Node* tokenIdentifer(std::string line, int lineCounter);
    bool hexChecker(char line);
    bool escapedCharacterChecker(char line);
    Node* getFirst() const { return first; }

    friend std::ostream& operator<< (std::ostream& out, const linklist& LL);



    private:
    Node* first = nullptr;
    Node* last = nullptr;
    int size = 0;
};