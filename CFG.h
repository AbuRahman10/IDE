//
// Created by aaditya on 12/10/23.
//

#ifndef PROGRAMMEEROPDRACHT1_CFG_H
#define PROGRAMMEEROPDRACHT1_CFG_H
#include "string"
#include "vector"
#include "iostream"
#include "algorithm"
#include "json.hpp"
#include "fstream"
#include "Lexer.h"

using json = nlohmann::json;
using namespace std;

class CFG {
private:
    std::vector<std::string> variables;
    std::vector<std::string> terminals;
    std::vector<std::pair<std::string,std::vector<std::string>>> productions;
    std::string startSymbol;
    json j;
public:
    CFG(const std::string& filename);

    CFG() = default;

    void setV(const std::vector<std::string> &v);

    void setT(const std::vector<std::string> &t);

    void setP(const std::vector<std::pair<std::string, std::vector<std::string>>> &p);

    void setS(const std::string &s);

    const std::vector<std::pair<std::string, std::vector<std::string>>> &getProductions() const;

    const std::string &getStartSymbol() const;

    const std::vector<std::string> &getVariables() const;

    const std::vector<std::string> &getTerminals() const;

    void print();

    bool parse(vector<Token> &tokens);
};


#endif //PROGRAMMEEROPDRACHT1_CFG_H
