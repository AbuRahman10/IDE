#include <iostream>
#include "SLR.h"
#include "CFG.h"
#include <sstream>
#include "Lexer.h"
int main() {
/*    CFG cfg("package.json");
    SLR slr(cfg);
    slr.closure();
    slr.goto_constructor();
    slr.creating_parsing_table();*/
    ifstream our_file("./input_code.txt");
    stringstream buffer;
    buffer << our_file.rdbuf();
    string code = buffer.str();
    Lexer tokenizer(code);
    vector<Token> our_tokens = tokenizer.tokenize();
    for (int i = 0; i < our_tokens.size(); ++i) {
        cout << "Token: "<< to_string(our_tokens[i].type) << " Value: " << our_tokens[i].word << " at line :"<<our_tokens[i].line<< " at column: "<<our_tokens[i].column<< endl;
    }
    return 0;
}