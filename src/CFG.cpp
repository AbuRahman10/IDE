//
// Created by aaditya on 12/10/23.
//

#include "CFG.h"
#include "SLR.h"
#include "regex"
CFG::CFG(const std::string& filename){
    std::ifstream input(filename);
    input >> j;
    for(const auto& i : j["Variables"]){
        variables.push_back(i);
    }
    std::sort(variables.begin(), variables.end());
    for(const auto& i : j["Terminals"]){
        terminals.push_back(i);
    }
    std::sort(terminals.begin(), terminals.end());
    for(const auto& i : j["Productions"]){
        productions.emplace_back(i["head"], i["body"]);
    }
    std::sort(productions.begin(), productions.end());
    startSymbol = j["Start"];
}

void CFG::print() {
    std::sort(variables.begin(), variables.end());
    std::sort(terminals.begin(), terminals.end());
    std::sort(productions.begin(), productions.end());
    std::cout << "V = {";
    for(int i = 0; i < variables.size(); i++){
        if(i != variables.size() - 1){
            std::cout << variables[i] << ", ";
        } else{
            std::cout << variables[i] << "}" << std::endl;
        }
    }
    std::cout << "T = {";
    for(int i = 0; i < terminals.size(); i++){
        if(i != terminals.size() - 1){
            std::cout << terminals[i] << ", ";
        } else{
            std::cout << terminals[i] << "}" << std::endl;
        }
    }
    std::cout << "P = {" << std::endl;
    for(const auto& i : productions){

        if(!i.second.empty()){
            for(int k = 0; k < i.second.size(); k++){
                std::cout << "\t" << i.first << " -> `";
                std::cout << i.second[k] << "`" << std::endl;
            }
        }else{
            std::cout << "\t" << i.first << " -> ``" << std::endl;

        }

    }
    std::cout << "}" << std::endl;
    std::cout << "S = " << startSymbol << std::endl;
}

void CFG::setV(const std::vector<std::string> &v) {
    CFG::variables = v;
}

void CFG::setT(const std::vector<std::string> &t) {
    CFG::terminals = t;
}

void CFG::setP(const std::vector<std::pair<std::string, std::vector<std::string>>> &p) {
    CFG::productions = p;
}

void CFG::setS(const std::string &s) {
    CFG::startSymbol = s;
}

const std::vector<std::pair<std::string, std::vector<std::string>>> &CFG::getProductions() const {
    return productions;
}

const std::string &CFG::getStartSymbol() const {
    return startSymbol;
}

const std::vector<std::string> &CFG::getVariables() const {
    return variables;
}

const std::vector<std::string> &CFG::getTerminals() const {
    return terminals;
}

vector<tuple<std::string, int, bool>> CFG::parse(vector<vector<Token>> &token, SLR &parser) {
    std::vector<std::pair<std::pair<string,int>,bool>> accept;
    std::vector<int> input_size;
    /*if(token.empty()){
        return {"",true};
    }*/
    for (int i = 0; i < token.size(); i++) {
        vector<std::pair<string,int>> input;
        vector<string> stack_input;
        string dataType;
        string keyword;
        string data_structure;
        input.clear();
        input.reserve(token[i].size());
        //regex identifier("[a-zA-Z][a-zA-Z0-9_]*");
        regex integer("[+-]?[0-9]+");
        // has to be raw cuz of escape sequence
        regex str(R"("(?:\\.|[^"\\])*")");
        // has to be raw cuz of escape sequence
        regex ch("'(?:\\.|[^\\'])'");
        regex ch1("'[0-9]+'");
        for (const Token& t : token[i])
        {
            string checking_token = t.word;
            if(t.typeToString() == "D"){
                dataType = t.word;
            }
            if(t.typeToString() == "K"){
                if(t.word == "if") { keyword = "if";}
                else if(t.word == "while") { keyword = "while";}
            }
            if(t.typeToString() == "N"){
                std::regex pattern("[a-zA-Z_][a-zA-Z0-9_]*");
                if(regex_match(t.word, pattern)){
                    input.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*",t.pos);
                    stack_input.emplace_back("[a-zA-Z_][a-zA-Z0-9_]*");
                } else{
                    input.emplace_back(t.word,t.pos);
                    stack_input.emplace_back(t.word);
                }
            } else if(t.typeToString() == "V"){
                if(dataType == "int"){
                    if(regex_match(t.word, integer)){
                        input.emplace_back("[+-]?[0-9]+",t.pos);
                        stack_input.emplace_back("[+-]?[0-9]+");
                    }
                    else if((keyword == "if" || keyword == "while") && (t.word == "true" || t.word == "false")){
                        input.emplace_back(t.word,t.pos);
                        stack_input.emplace_back(t.word);
                    }
                }else if(dataType == "string"){
                    if(regex_match(t.word, str) || t.word.empty()){
                        // has to be raw cuz of escape sequence
                        input.emplace_back(R"("(?:\\.|[^"\\])*")",t.pos);
                        stack_input.emplace_back(R"("(?:\\.|[^"\\])*")");
                    }
                    else {
                        input.emplace_back(t.word,t.pos);
                        stack_input.emplace_back(t.word);
                    }
                } else if(dataType == "char"){
                    if(regex_match(t.word, ch)){
                        // has to be raw cuz of escape sequence
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if(regex_match(t.word, ch1)){
                        input.emplace_back("'[0-9]+'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\n"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\t"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\r"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\b"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\f"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\a"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\v"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if (t.word == "\?"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }else if(t.word == "\0"){
                        input.emplace_back("'(?:\\.|[^\\'])'",t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }
                    else {
                        input.emplace_back(t.word,t.pos);
                        stack_input.emplace_back("'(?:\\.|[^\\'])'");
                    }
                } else if (dataType == "bool"){
                    input.emplace_back(t.word,t.pos);
                    stack_input.emplace_back("'(?:\\.|[^\\'])'");
                }
                else{
                    input.emplace_back(t.word,t.pos);
                    stack_input.emplace_back("'(?:\\.|[^\\'])'");
                }
            }
            else{
                input.emplace_back(t.word,t.pos);
                stack_input.emplace_back("'(?:\\.|[^\\'])'");
            }
        }
        input.emplace_back("$",0);
        input_size.push_back(lex.pos_dollar[i]);
        pair<vector<string>,vector<string>> stack_value = {{"0"},stack_input};
        accept.push_back(parser.slr_parsing(input,stack_value));
    }
    vector<tuple<string,int,bool>> t;
    t.reserve(accept.size());
    for(int i = 0; i < accept.size(); i++){
        int pos = 0;
        if(accept[i].second){
            pos = input_size[i];
            t.emplace_back(accept[i].first.first,pos,accept[i].second);
        } else{
            t.emplace_back(accept[i].first.first,accept[i].first.second,accept[i].second);
        }
    }
    return t;
}

SLR CFG::createTable() {
    SLR parser(*this);
    parser.closure();
    parser.goto_constructor();
    parser.creating_parsing_table();
    return parser;
}

void CFG::setLex(const Lexer &lex) {
    CFG::lex = lex;
}
