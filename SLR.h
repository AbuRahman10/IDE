
//
// Created by aaditya on 24/11/23.
//

#ifndef IDE_SLR_H
#define IDE_SLR_H

#include "CFG.h"
#include "algorithm"
using namespace std;
using production = pair<string,vector<string>>;
//production gives the production rule and int is for the position of dot
using I = vector<pair<production,int>>;

class SLR : public CFG {
private:
    vector<production> augmented_production;
    vector<I> goto_s;
    string StartSymbolSlr;
    vector<vector<string>> I_vector;
    I CL;
public:
    SLR();

    SLR(const CFG& cfg);

    void closure();

    void variable_check(I& i0);

    bool production_checker(const string& head, I& i0);

    void goTo(const I &random_I, string variable, int number);

    void remaining_Variables(const I &random_I);

};


#endif //IDE_SLR_H