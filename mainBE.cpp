#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"
int main() {
/*    CFG cfg("grammar_works.json");
    SLR slr(cfg);
    slr.closure();
    slr.goto_constructor();
    slr.creating_parsing_table();
    vector<string> input = {"char","[a-zA-Z][a-zA-Z0-9]*","=","'[0-9]+'",";","$"};
    pair<vector<string>,vector<string>> stack_value = {{"0"},input};
    cout << boolalpha << slr.slr_parsing(input,stack_value);*/
    string code = "vector<string> some = {1,2,3,4,5};";
    Lexer ome (code);
    auto get = ome.tokenize();
    CFG some;
    auto result = some.parse(get);
    return 0;
}