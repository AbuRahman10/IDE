#include <iostream>
#include "SLR.h"
#include "CFG.h"
#include <sstream>
#include "Lexer.h"
int main() {
    std::ifstream file("../input_code.txt");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    // Gebruik dit shared ptr in de parser !
    std::shared_ptr<Lexer> lexical_analyser = make_shared<Lexer>(code);
    auto get_tokens = lexical_analyser->tokenize();
    for (int i = 0; i < get_tokens.size(); ++i) {
        cout << "TOKEN: " << get_tokens[i].type << " Value: " << get_tokens[i].word << endl;
    }
    return 0;
}