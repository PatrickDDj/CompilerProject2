//
//  main.cpp
//  CompilerProject2
//
//  Created by PatrickDD on 2021/4/18.
//

#include "GrammarAnalyzer.h"
#include "LexicalAnalyzer.h"


int main(int argc, const char * argv[]) {
    LexicalAnalyzer lex("p2.txt");

    GrammarAnalyzer grammar(lex.lex_result);
    grammar.draw_AST("AST.txt");

}
