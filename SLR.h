
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
    vector<I> collectionI;
    string StartSymbolSlr;


public:
    SLR();

    SLR(const CFG& cfg);


    void closure();

    void variable_check(I& i0);

    bool production_checker(const string& head, I& i0);

};


#endif //IDE_SLR_H