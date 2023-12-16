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
    //first terminal
    for(const auto &f : this->getTerminals()){
        first[f] = {f};
    }
    //variables
    for(const auto &f : this->getVariables()){
        createFirst(f);
    }
    //create empty set for follow
    for(const auto &vars : this->getVariables()){
        if(vars == this->getStartSymbol()){
            follows[vars] = {"$"};
        } else{
            follows[vars] = {};
        }
    }
    //create follow
    for(const auto& prod : this->getProductions()){
        for(const auto& body : prod.second){
            auto it = find(this->getVariables().begin(), this->getVariables().end(), body);
            if(it != this->getVariables().end()){
                createFollow(body);
            }
        }
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
    vector<string> heads;
    for(int i = 0; i < i0.size(); i++){
        auto it = find(this->getVariables().begin(), this->getVariables().end(), i0[i].first.second[i0[i].second]);
        auto h = find(heads.begin(),heads.end(),i0[i].first.second[i0[i].second]);
        if (it != this->getVariables().end() && h == heads.end()) {
            for(const auto& k : augmented_production){
                if(k.first == *it){
                    i0.emplace_back(k, 0);
                }
            }
            heads.push_back(*it);
            i = 0;
        }
    }
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



bool SLR::slr_parsing(vector<string> &input,pair<vector<string>,vector<string>> &stack_value) {

    static bool accept = false;
    string ss = input[0];
    string action = parsing_table[stoi(stack_value.first[stack_value.first.size()-1])].first[ss];

    if (action == "accept") {
        if(input.size() > 1){
            accept = false;
        }else{
            accept = true;
        }
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

void SLR::createFirst(const string& symbol) {
    //first epsilon
    for(const auto &prod : this->getProductions()){
        //if head is symbol and body is epsilon , add epsilon to first
        if(prod.first == symbol){
            for(const auto &s : prod.second){
                if(s.empty()){
                    first[symbol] = {""};
                }
            }
        }
    }
    //third rule
    for(const auto &prod : this->getProductions()){
        if(prod.first == symbol){
            unsigned int epsilon_counter = 0;
            for(const auto &body : prod.second){
                //to check if y1 has epsilon as rule
                bool epsilon = false;
                if(!body.empty()){
                    //loop through first of body and add it to prod.first
                    if(body != symbol){
                        createFirst(body);
                    }
                    for(const auto &f : first[body]){
                        if(f.empty()){
                            epsilon = true;
                            epsilon_counter++;
                        } else{
                            first[prod.first].insert(f);
                        }
                    }
                }
                if(!epsilon){
                    break;
                }

            }
            if(epsilon_counter == prod.second.size()){
                first[prod.first].insert("");
            }
        }
    }

}

//creates follow for grammar
void SLR::createFollow(const string &variable) {
    for(const auto& prod : this->getProductions()){
        //loop through body
        for(unsigned int i = 0; i < prod.second.size(); i++){
            if(variable == prod.second[i]){
                vector<string> beta;
                for(unsigned int j = i+1; j < prod.second.size();j++){
                    beta.push_back(prod.second[j]);
                }
                set<string> firstBeta = firstOfString(beta);
                bool epsilon = false;
                for(const auto& b : firstBeta){
                    if(b.empty()){
                        epsilon = true;
                    }else{
                        follows[prod.second[i]].insert(b);
                    }
                }
                //if the last element or first of beta contain epsilon
                if(i == prod.second.size()-1 || epsilon){
                    createFollow(prod.first);
                    for(const auto &f : follows[prod.first]){
                        follows[variable].insert(f);
                    }
                }

            }
        }
    }

}
//helping function for follow
set<string> SLR::firstOfString(const vector<string> &beta) {
    set<string> firsts;
    int epsilon_counter = 0;
    for(const auto &s : beta){
        bool epsilon = false;
        for(const auto& f : first[s]){
            if(!f.empty()){
                firsts.insert(f);
            }else{
                epsilon = true;
                epsilon_counter++;
            }
        }
        if(!epsilon){
            break;
        }
    }
    if (epsilon_counter == beta.size()){
        firsts.insert("");
    }
    return firsts;
}
