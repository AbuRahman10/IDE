
//
// Created by aaditya on 24/11/23.
//

#ifndef IDE_SLR_H
#define IDE_SLR_H

#include "CFG.h"
#include "algorithm"
#include "vector"
using namespace std;
using production = pair<string,vector<string>>;
//production gives the production rule and int is for the position of dot
using I = vector<pair<production,int>>;

class SLR : public CFG {
private:
    vector<production> augmented_production;
    int number_of_i = 0;
    vector<I> goto_s;
    string StartSymbolSlr;
    //Collection of symbols which has not been implemented by goto
    vector<vector<string>> I_vector;
    vector<string> rem_variables;
    //closure
    I CL;
public:
    SLR();

    SLR(const CFG& cfg);

    void closure();

    void variable_check(I& i0);

    bool production_checker(const string& head, I& i0);

    void i_vector_maker(const I &i);

    void goto_constructor();

    bool i_vector_checker(string symbol, int number_i);

    void closure_checker(I &new_I, const string& head);

    void i_maker(const I& i, const string& symbol);

    bool check_for_constructed_rules(const pair<production,int>  &check_i);

};


#endif //IDE_SLR_H