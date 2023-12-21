#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"
int main() {
    ifstream input("SavedFile.txt");
    stringstream buffer;
    buffer << input.rdbuf();
    string code = buffer.str();
    Lexer ome (code);
    auto get = ome.tokenize();
    ome.print();
    CFG some("grammar.json");
    SLR slr = some.createTable();
    cout << boolalpha << some.parse(get,slr);
    return 0;
}