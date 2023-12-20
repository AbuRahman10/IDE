#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"
int main() {
    string code = "if(int i = 10 < 12 ){} else if(int i = 10 < 13){} else{int i = 0;}";
    Lexer ome (code);
    auto get = ome.tokenize();
    CFG some("grammar.json");
    SLR slr = some.createTable();
    cout << some.parse(get,slr);
    return 0;
}