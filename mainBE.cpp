#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"
int main() {
    string code = "int i = 0;";
    Lexer ome (code);
    auto get = ome.tokenize();
    CFG some;
    SLR slr = some.createTable();
    cout << some.parse(get,slr);
    return 0;

}