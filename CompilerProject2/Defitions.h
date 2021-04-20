//
//  Defitions.h
//  Compiler_Porject1
//
//  Created by PatrickDD on 2021/3/29.
//

#ifndef Defitions_h
#define Defitions_h
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int node_format_length = 2;
const vector<string> KEY_WORDS = {
    "main", "include", "void", "return",
    "int", "bool", "float", "double","char","string",
    
    "(", ")", "[", "]", "{", "}", ".", "#", "_", ",", ";", "'", "\"",
    
    "!", "&", "~", "^","|",
    "+", "-", "*", "/", "%",
    ">", "<", "=",
    "->", "++", "--",
    "<<", ">>",
    "<=", ">=", "==", "!=",
    "&&", "||",
    "+=", "-=", "/=","*=", "^=", "&=", "|=", "%=", "~=",
    
    "//",
    
    "Id",
    "Decimal_Number", //十进制
    "Hexademical_Number", //十六进制
    "Octal_Number", //八进制
    "String_Constant",
    
    "if", "else", "while", "for", "else if","break", "continue",
    "get", "put",
    "true", "false"
    
};

map<string, int> KEY_WORDS_MAP;

//map<string, int> Id_MAP;

#endif /* Defitions_h */
