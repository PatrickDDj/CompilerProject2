//
//  main.cpp
//  CompilerProject2
//
//  Created by PatrickDD on 2021/4/18.
//

#include "GrammarAnalyzer.h"



int main(int argc, const char * argv[]) {
    
    GrammarAnalyzer grammar("p2.txt");
    grammar.grammer_analysis();
    grammar.print_AST();
    
    
    
}
