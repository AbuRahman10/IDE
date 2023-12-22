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
    CFG some("grammar.json");
    some.setLex(ome);
    SLR slr = some.createTable();
    vector<tuple<string,int,bool>> t = some.parse(get,slr);
    for(const auto& i : t){
        // Access elements of the tuple using std::get
        string word = std::get<0>(i);
        int char_point = std::get<1>(i);
        bool accept = std::get<2>(i);
        if(!accept){
            std::cout << word << "\t" << char_point << std::endl;
        }
    }

    return 0;
}