#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"
int main() {
    string code = "if(true){}else_if(true){}else{}";
    Lexer ome (code);
    auto get = ome.tokenize();
    CFG some("grammar.json");
    SLR slr = some.createTable();
    cout << some.parse(get,slr);
    return 0;
}