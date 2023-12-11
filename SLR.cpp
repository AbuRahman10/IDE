//
// Created by aaditya on 24/11/23.
//

#include "SLR.h"

SLR::SLR(CFG &cfg) {
    this->setV(cfg.getVariables());
    this->setP(cfg.getProductions());
    this->setS(cfg.getStartSymbol());
    this->setT(cfg.getTerminals());
    augmented_production = this->getProductions();
    augmented_production.push_back({this->getStartSymbol()+"'",{this->getStartSymbol()}});
    StartSymbolSlr = this->getStartSymbol()+"'";
    //create empty set for follow
    for(const auto &vars : this->getVariables()){
        if(vars == this->getStartSymbol()){
            follows[vars] = {"$"};
        } else{
            follows[vars] = {};
        }
    }
    //create follow
    for(const auto &vars : this->getVariables()){
        createFollow(vars);
    }
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

//helping function closure
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

//checks if the parameter head is in i0(also a helping function for closure)
bool SLR::production_checker(const string& head, I &i0) {
    for(const auto& i : i0) {
        if (i.first.first == head) {
            return true;
        }
    }
    return false;
}

//-------------------- implementations of goto functions------------------------------------
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
    for(const auto& closure : augmented_production){
        if(closure.first == head){
            new_I.emplace_back(closure,0);
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


//---------- implementation of table----------------------------------


void SLR::creating_parsing_table() {
    //initialize
    for(int i = 0; i < goto_s.size(); i++){
        action action1;
        for(const auto &terminal : this->getTerminals()){
            action1[terminal] = "";
        }
        action1["$"] = "";
        go go1;
        for(const auto &vars : this->getVariables()){
            go1[vars] = 0;
        }
        parsing_table.emplace_back(action1,go1);
    }
    //shift operation for actions
    for(int i = 0; i < goto_s.size(); i++){
        for(const auto &rule : goto_s[i]){
            //check if there is a terminal followed by dot
            auto it = find(this->getTerminals().begin(), this->getTerminals().end(), rule.first.second[rule.second]);
            if(it != this->getTerminals().end()){
                pair<production,int> rule_to_check = {rule.first,rule.second+1};
                parsing_table[i].first[*it] = "s" + to_string(shift_check(rule_to_check));
            }
        }
    }
    //shift operation for variables
    for(int i = 0; i < goto_s.size(); i++){
        vector<string> implemented_variables;
        for(const auto &rule : goto_s[i]){
            //check if there is a terminal followed by dot && is not already implemented
            auto varit = find(implemented_variables.begin(),implemented_variables.end(),rule.first.second[rule.second]);
            auto it = find(this->getVariables().begin(), this->getVariables().end(), rule.first.second[rule.second]);
            if(it != this->getVariables().end() && varit == implemented_variables.end()){
                pair<production,int> rule_to_check = {rule.first,rule.second+1};
                parsing_table[i].second[*it] = shift_check(rule_to_check);
                implemented_variables.push_back(*it);
            }
        }
    }
    //find the accepting state && reduce for actions
    for(int i = 0; i < goto_s.size();i++){
        for(const auto &rule : goto_s[i]){
            if(rule.first.first == StartSymbolSlr && rule.second == rule.first.second.size()){
                parsing_table[i].first["$"] = "accept";
            }
        }
    }
    //reduce for actions
    for(int i = 0; i < goto_s.size(); i++){
        for(const auto &rule : goto_s[i]){
            //if dot is at the end
            if(rule.second == rule.first.second.size()){
                //find the rule number
                int rule_number = 0;
                for(int k = 0; k < this->getProductions().size();k++){
                    //if same rule
                    if(rule.first.first == this->getProductions()[k].first && equal(rule.first.second.begin(),rule.first.second.end(),this->getProductions()[k].second.begin())){
                        rule_number = k;
                    }
                }
                //fill the reduce
                for(auto const &f : follows[rule.first.first]){
                    parsing_table[i].first[f] = "r" + to_string(rule_number);
                }
            }
        }
    }

}

//returns the i  number where dot was shifted for that symbol
int SLR::shift_check(const pair<production,int>& rule_to_check) {
    for(int i = 0; i < goto_s.size(); i++){
        for(auto const &rule : goto_s[i]){
            if(equal(rule.first.second.begin(),rule.first.second.end(), rule_to_check.first.second.begin()) && rule.first.first == rule_to_check.first.first && rule_to_check.second == rule.second){
                return i;
            }
        }
    }
    return 0;
}

//creates follow for grammar(rules for epsilon is not implemented, will implement it later if necessary)
void SLR::createFollow(const string& variable) {
    static map<string,bool> new_terminal;
    //set all variables in new_terminal to false
    for(auto& vars : this->getVariables()){
        new_terminal[vars] = false;
    }
    //terminals
    for(const auto &rule : this->getProductions()){
        for(int i = 0; i < rule.second.size(); i++){
            if(rule.second[i] == variable && i != rule.second.size()-1){
                auto it = find(this->getTerminals().begin(), this->getTerminals().end(), rule.second[i+1]);
                if(it != this->getTerminals().end()){
                    follows[variable].insert(*it);
                    new_terminal[variable] = true;
                }
            }
        }
    }
    //variable
    for(const auto &rule : this->getProductions()){
        for(int i = 0; i < rule.second.size(); i++){
            if(i == rule.second.size()-1 && rule.second[i] == variable){
                for(const auto &follow_var : follows[rule.first]){
                    follows[variable].insert(follow_var);
                    new_terminal[variable] = true;
                }
            }
        }
    }
    //check if head has new terminals && use recursion
    for(const auto &vars : this->getVariables()){
        for(const auto &rule : this->getProductions()){
            for(int i = 0; i < rule.second.size(); i++){
                if(i == rule.second.size()-1 && rule.second[i] == vars){
                    if(new_terminal[rule.first]){
                        createFollow(vars);
                    }
                }
            }
        }
    }

}

bool SLR::slr_parsing(vector<string> &input,pair<vector<string>,vector<string>> &stack_value) {

    static bool accept = false;
    string ss = input[0];
    string action = parsing_table[stoi(stack_value.first[stack_value.first.size()-1])].first[ss];

    if (action == "accept") {
        accept = true;
    }
    if (action[0] == 's') {
        stack_value.first.push_back(*input.begin());
        string temp;
        for (int i = 1; i < action.size(); i++) {
            temp += action[i];
        }
        stack_value.first.push_back(temp);
        input.erase(input.begin());
        stack_value.second.erase(stack_value.second.begin());
        slr_parsing(input,stack_value);
    }
    if (action[0] == 'r') {
        string temp;
        for (int i = 1; i < action.size(); i++) {
            temp += action[i];
        }
        unsigned int length = 2 * this->getProductions()[stoi(temp)].second.size();
        for (int i = 0; i < length; i++) {
            stack_value.first.pop_back();
        }
        stack_value.first.push_back(this->getProductions()[stoi(temp)].first);
        int index = stoi(stack_value.first[stack_value.first.size() - 2]);
        stack_value.first.push_back(
                to_string(parsing_table[index].second[stack_value.first[stack_value.first.size() - 1]]));
        slr_parsing(input,stack_value);

    }
    if (action.empty() or action == "0") {
        accept = false;
    }
    return accept;
}
