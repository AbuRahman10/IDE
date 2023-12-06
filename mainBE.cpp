#include <iostream>
#include "SLR.h"
#include "CFG.h"
#include <sstream>
#include "Lexer.h"
int main() {
    CFG cfg("package.json");
    SLR slr(cfg);
    slr.closure();
    slr.goto_constructor();
    slr.creating_parsing_table();
}