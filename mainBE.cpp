#include <iostream>
#include "SLR.h"
#include "CFG.h"
#include "regex"
#include <sstream>
#include "Lexer.h"
int main() {
    CFG cfg("grammar_works.json");
    SLR slr(cfg);
    slr.closure();
    slr.goto_constructor();
    slr.creating_parsing_table();
    vector<string> input = {"char","[a-zA-Z][a-zA-Z0-9]*","=","'[0-9]+'",";","$"};
    pair<vector<string>,vector<string>> stack_value = {{"0"},input};
    cout << boolalpha << slr.slr_parsing(input,stack_value);
    return 0;
}