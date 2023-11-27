//
// Created by aaditya on 24/11/23.
//

#include "SLR.h"

SLR::SLR() {
    augmented_production = this->getProductions();
    augmented_production.push_back({this->getStartSymbol()+"'",{this->getStartSymbol()}});
    StartSymbolSlr = this->getStartSymbol()+"'";
}


void SLR::closure() {

    I initial_i;
    for(const auto& i : augmented_production){
        if(i.first == StartSymbolSlr){
            initial_i.emplace_back(i,0);
        }
    }
    variable_check(initial_i);



}

void SLR::variable_check(I &i0) {
    for(auto i : i0){
        auto it = find(this->getVariables().begin(), this->getVariables().end(), i.first.second[i.second]);
        if (it != this->getVariables().end()) {
            if (!production_checker(*it, i0)){
                for(const auto& k : augmented_production){
                    if(k.first == *it){
                        i0.emplace_back(k, 0);
                    }
                }
                variable_check(i0);
            }
        }
    }
}

bool SLR::production_checker(const string& head, I &i0) {
    for(const auto& i : i0) {
        if (i.first.first == head) {
            return true;
        }
    }
    return false;
}

SLR::SLR(const CFG &cfg) {
    this->setV(cfg.getVariables());
    this->setP(cfg.getProductions());
    this->setS(cfg.getStartSymbol());
    this->setT(cfg.getTerminals());
    augmented_production = this->getProductions();
    augmented_production.push_back({this->getStartSymbol()+"'",{this->getStartSymbol()}});
    StartSymbolSlr = this->getStartSymbol()+"'";
}
