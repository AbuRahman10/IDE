//
// Created by aaditya on 12/10/23.
//

#include "CFG.h"
#include "SLR.h"

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

bool CFG::parse(vector<Token> &tokens)
{
    if(tokens.empty()){
        return true;
    }
    CFG cfg("grammar_works.json");
    vector<string> input;
    string dataType;
    input.reserve(tokens.size());
    regex identifier("[a-zA-Z][a-zA-Z0-9]*");
    regex integer("[0-9]+");
    regex str("\"[a-zA-Z0-9]*\"");
    regex ch("'[a-zA-Z0-9]'");
    regex ch1("'[0-9]+'");
    for (const Token& token : tokens)
    {
        if(token.typeToString() == "D"){
            dataType = token.word;
        }
        if(token.typeToString() == "N"){
            std::regex pattern("[a-zA-Z][a-zA-Z0-9]*");
            if(regex_match(token.word,pattern)){
                input.emplace_back("[a-zA-Z][a-zA-Z0-9]*");
            } else{
                input.emplace_back(token.word);
            }
        } else if(token.typeToString() == "V"){
            if(dataType == "int"){
                if(regex_match(token.word,integer)){
                    input.emplace_back("[0-9]+");
                }
            }else if(dataType == "string"){
                if(regex_match(token.word,str) || token.word.empty()){
                    input.emplace_back("\"[a-zA-Z0-9]*\"");
                }
            } else if(dataType == "char"){
                if(regex_match(token.word,ch)){
                    input.emplace_back("'[a-zA-Z0-9]'");
                }else if(regex_match(token.word,ch1)){
                    input.emplace_back("'[0-9]+'");
                }
            } else{
                input.push_back(token.word);
            }
        }
        else{
            input.push_back(token.word);
        }
    }
    input.emplace_back("$");
    SLR parser(cfg);
    parser.closure();
    parser.goto_constructor();
    parser.creating_parsing_table();
    pair<vector<string>,vector<string>> stack_value = {{"0"},input};
    bool accept = parser.slr_parsing(input,stack_value);
    return accept;
}
