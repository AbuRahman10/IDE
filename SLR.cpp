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
    goto_s.push_back(initial_i);
    CL = initial_i;
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

void SLR::goTo(const I &random_I, string variable, int number) {

    // nieuwe I maken
    I new_I;
    // loopen door een meegegeven I (in het begin is het I0)
    for (const auto &rule : random_I) {
        // loopen door de body van elke rule in I
        for (int i = 0; i < rule.first.second.size(); i++) {
            // als de meegegeven variabele == variabele na puntje -> voeg toe aan new_I en verschuif puntje
            if (variable == rule.first.second[i] && i == rule.second) {
                int pos = rule.second + 1;
                pair<production,int> new_var = {rule.first,pos};
                new_I.push_back(new_var);
            }
        }

        for (const auto &prod : new_I) {
            auto it = find(this->getVariables().begin(), this->getVariables().end(), prod.first.second[prod.second]);
            if (it != this->getVariables().end()){
                for (const auto &rule : CL) {
                    if (rule.first.first == *it) {
                        new_I.push_back(rule);
                    }
                }
            }
        }

    }

    for (auto &symbol : I_vector[number]) {
        if (symbol == variable) {
            I_vector[number].erase(std::remove(I_vector[number].begin(), I_vector[number].end(), variable), I_vector[number].end());
        }
    }

}

void SLR::remaining_Variables(const I &random_I) {

    for (auto symbol : random_I) {
        vector<string> temp;
        auto it = find(temp.begin(),temp.end(), symbol.first.second[symbol.second]);
        if (it == temp.end()) {
            temp.push_back(symbol.first.second[symbol.second]);
        }
    }




}
