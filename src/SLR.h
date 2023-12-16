
//
// Created by aaditya on 24/11/23.
//

#ifndef IDE_SLR_H
#define IDE_SLR_H

#include "CFG.h"
#include "algorithm"
#include "vector"
#include "map"
#include "iostream"
#include "set"
using namespace std;
using production = pair<string,vector<string>>;
//production gives the production rule and int is for the position of dot
using I = vector<pair<production,int>>;
//parsing table
using action = map<string,string>;
using go = map<string,int>;

class SLR : public CFG {
private:
    vector<production> augmented_production;
    int number_of_i = 0;
    vector<I> goto_s;
    string StartSymbolSlr;
    //closure
    I CL;
    vector<pair<action,go>> parsing_table;
    map<string,set<string>> follows;
    map<string,set<string>> first;
    //these variables are not part of the slr algo but they are here to help the implementation
    vector<vector<string>> I_vector;
    vector<string> rem_variables;
public:

    SLR(CFG& cfg);

    void closure();

    void variable_check(I& i0);

    void i_vector_maker(const I &i);

    void goto_constructor();

    bool i_vector_checker(string symbol, int number_i);

    void closure_checker(I &new_I, const string& head);

    void i_maker(const I& i, const string& symbol);

    bool check_for_constructed_rules(const pair<production,int>  &check_i);

    void creating_parsing_table();

    int shift_check(const pair<production,int>& rule_to_check);

    void createFollow(const string &variable);

    void createFirst(const string& symbol);

    set<string> firstOfString(const vector<string> &beta);

    bool slr_parsing(vector<string> &input, pair<vector<string>,vector<string>> &stack_value);
};


#endif //IDE_SLR_H