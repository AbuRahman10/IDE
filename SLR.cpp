//
// Created by aaditya on 24/11/23.
//

#include "SLR.h"

SLR::SLR() {
    augmented_production = this->getProductions();
    augmented_production.push_back({this->getStartSymbol()+"'",{this->getStartSymbol()}});
    StartSymbolSlr = this->getStartSymbol()+"'";
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
    number_of_i++;
}

//hulpfunctie closure
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

//checks if the parameter head is in i0(also i hulpfunctie for closure)
bool SLR::production_checker(const string& head, I &i0) {
    for(const auto& i : i0) {
        if (i.first.first == head) {
            return true;
        }
    }
    return false;
}
//constructs goto functions
void SLR::goto_constructor() {
    for (int i = 0; i < number_of_i; i++) {
        i_vector_maker(goto_s[i]);
        for (const auto &rule: goto_s[i]) {
            // lopen door de body van elke rule in I
            for (int k = 0; k < rule.first.second.size(); k++) {
                // als de meegegeven variabele == variabele na puntje en als de variable niet goto function heeft, maak het aan
                if (k == rule.second && i_vector_checker(rule.first.second[k],i)) {
                    //kijk of de gegeven regel al bestaat
                    pair<production,int> temp_rule = {rule.first,rule.second+1};
                    if(!check_for_constructed_rules(temp_rule)){
                        i_maker(goto_s[i],rule.first.second[k]);
                        //remove the symbol from I_vector
                        for(auto &i_vector : I_vector[i]){
                            if(i_vector == rule.first.second[k]){
                                I_vector[i].erase(std::remove(I_vector[i].begin(), I_vector[i].end(), i_vector));
                            }
                        }

                    }
                }
            }
        }
    }
}

//makes a vector for I_vector
void SLR::i_vector_maker(const I &random_I) {
    vector<string>temp;
    for (const auto &rule : random_I) {
        // lopen door de body van elke rule in I
        for (int i = 0; i < rule.first.second.size(); i++) {
            // als de meegegeven variabele == variabele na puntje -> voeg toe aan temp
            auto it = find(temp.begin(),temp.end(),rule.first.second[i]);
            if(it == temp.end()){
                if (i == rule.second) {
                    temp.push_back(rule.first.second[i]);
                }
            }
        }
    }
    I_vector.push_back(temp);
}

//checks if a given symbol has a goto for given i
bool SLR::i_vector_checker(string symbol, int number_i) {
    for(const auto &k : I_vector[number_i]){
        if(k == symbol){
            return true;
        }
    }
    return false;
}

//generates new i's
void SLR::i_maker(const I &i, const string& symbol) {
    I new_i;
    for(const auto &j : i){
        //check if the symbol comes after .
        if(j.first.second[j.second] == symbol){
            //change pos of .
            int pos = j.second+1;
            new_i.emplace_back(j.first,pos);
            //check if the next symbol is a variable
            auto it = find(this->getVariables().begin(), this->getVariables().end(),j.first.second[j.second+1]);
            if(it != this->getVariables().end()){
                closure_checker(new_i,j.first.second[j.second+1]);
                rem_variables = {};
            }

        }
    }
    goto_s.push_back(new_i);
    number_of_i++;
}

//recursive closure function, if we can apply closue on a rule then this function does it
void SLR::closure_checker(I &new_I, const string& head) {
    for(const auto& closure: CL){
        if(closure.first.first == head){
            new_I.push_back(closure);
        }
    }
    rem_variables.push_back(head);
    //check if we can apply closure in the just added rules
    for(int i = 0; i < new_I.size(); i++){
        auto it = find(rem_variables.begin(),rem_variables.end(),new_I[i].first.second[new_I[i].second]);
        auto is_variable = find(this->getVariables().begin(), this->getVariables().end(),new_I[i].first.second[new_I[i].second]);
        if(it == rem_variables.end() && is_variable != this->getVariables().end()){
            closure_checker(new_I,new_I[i].first.second[new_I[i].second]);
        }
    }

}

//check if the create rule already exists
bool SLR::check_for_constructed_rules(const pair<production,int> &check_i) {
    for(const auto &i : goto_s){
        for(const auto &rules : i){
            if(equal(rules.first.second.begin(),rules.first.second.end(), check_i.first.second.begin()) && check_i.first.first == rules.first.first && check_i.second == rules.second){
                return true;
            }
        }
    }
    return false;
}
