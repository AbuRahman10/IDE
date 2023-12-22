#include <iostream>
#include "src/SLR.h"
#include "src/CFG.h"
#include "regex"
#include <sstream>
#include "src/Lexer.h"

// ANSI Color Codes
const std::string RED = "\033";     // Red
const std::string GREEN = "\033[32m";   // Green
const std::string YELLOW = "\033[33m";  // Yellow
const std::string RESET = "\033[0m";    // Reset to default color
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
    vector<int> errors;
    for(const auto& i : t){
        // Access elements of the tuple using std::get
        if(!std::get<2>(i)){
            //get<1> gives the char number where the grammar goes wrong
            errors.push_back(std::get<1>(i));
        }
    }
    //for row and column
    int row = 0;
    int column = 0;
    for(int i = 0; i < code.size(); i++){
        auto it = find(errors.begin(),errors.end(),i);
        if(it != errors.end()){
            std::cout << "(" << row+1 << "," << column << ")" << std::endl;
        }
        if(code[i] == '\n'){
            row++;
            column = 0;
        }else{
            column++;
        }
    }

    return 0;
}