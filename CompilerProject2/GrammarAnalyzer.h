//
//  GrammarAnalyzer.h
//  CompilerProject2
//
//  Created by PatrickDD on 2021/4/18.
//

#include "LexicalAnalyzer.h"

#ifndef GrammarAnalyzer_h
#define GrammarAnalyzer_h

// Program -> int main Block
// Block -> { Statements }
// Statements -> Statement Statements | Empty
// Statement -> Assignment ; | Declaration ; | break ; | continue; | DO_WHILE | IF | WHILE | GET ; | PUT ; | Calculation_Assignment ;
// IF -> if ( Bool ) Block |
//       if ( Bool ) Block else Block |
//       if ( Bool ) Block ELSE_IF else Block
// ELSE_IF -> else if ( Bool ) Block ELSE_IF | Empty
// GET -> get ( Identifier )
// PUT -> put ( Expression )
// Assignment -> Identifier = Expression
// Declaration -> Type Descriptions
// Descriptions -> Description | Description , Descriptions
// Description -> Identifier | Assignment
// Type -> bool | char | int | double | float | string
// Expression -> Expression + Term | Expression - Term | Term
// Term -> Term * Factor | Term / Factor | Factor
// Factor -> Number | Identifier | ( Expression )
// Bool -> Expression | Expression Op Expression
// Op -> == | >= | > | < | <=
// Number -> Decimal_Number | Octal_Number | Hexademical_Number



class Node{
public:
    string Component;
    vector<Node> sons;
    
    Node(string Component){
        this->Component = Component;
    }
    
    void add_son(Node son){
        sons.push_back(son);
    }
    
    bool is_terminal(){
        return sons.size() == 0;
    }
    
    bool is_Empty(){
        return Component == "Empty";
    }
    
};


class GrammarAnalyzer {
    
public:
    int cur = 0;
//    Node *root = new Node("");
    Node root;
    vector<pair<vector<string>, int> > lex_result;
    
    GrammarAnalyzer(string code_path):root("Root"){
        LexicalAnalyzer lex(code_path);
        lex_result = lex.lex_result;
//        root = new Node("Root");
    }
    
    void print_AST(){
        print_Node(root);
    }
    
    void print_Node(Node node){
        printf("Node : %s\n", node.Component.c_str());
        for(Node son : node.sons){
            print_Node(son);
        }
    }
    
    
    string get_word(){
        return lex_result[cur].first[0];
    }
    
    string get_next_word(){
        return lex_result[cur+1].first[0];
    }
    
    bool is_Type(){
        string word = get_word();
        return (word=="int" || word=="char" || word=="bool" ||
                word=="string" || word=="double" || word=="float");
    }
    
    bool is_Identifier(){
        int word_id = lex_result[cur].second;
        return KEY_WORDS[word_id-1] == "Identifier";
    }
    
    bool is_Number(){
        int word_id = lex_result[cur].second;
        string word_type = KEY_WORDS[word_id-1];
        return (word_type=="Octal_Number" || word_type=="Decimal_Number" || word_type=="Hexademical_Number");
    }
    
    bool is_Compare_Op(){
        string word = get_word();
        return (word=="==" || word=="<=" || word=="<" || word==">=" || word==">");
    }
    
    void grammer_analysis(){
        Node Program = proc_Program();
        root.add_son(Program);
    }
    
    Node proc_Program(){
        Node Program("Program");
        
        if(get_word() == "int"){
//            cout << "Miss 'int' at the beginning!!!" << endl;
            cur++;
            Program.add_son(Node("int"));
        }
        if(get_word() == "main"){
//            cout << "Miss 'main' at the beginning!!!" << endl;
            cur++;
            Program.add_son(Node("main"));
        }

        Node Block = proc_Block();
        Program.add_son(Block);

        return Program;
    }
    
    Node proc_Block(){
        Node Block("Block");
        if(get_word() == "{"){
            Block.add_son(Node("{"));
            cur++;
        }

        if(!(get_word() == "}")){
            Block.add_son(proc_Statements());
        }
        
        if(get_word() == "}"){
            Block.add_son(Node("}"));
            cur++;
        }
        return Block;
    }
    
    Node proc_Statements(){
        Node Statements("Statements");
        
        Statements.add_son(proc_Statement());

        while(!(get_word() == "}")){
            Statements.add_son(proc_Statement());
        }
            
        return Statements;
    }
    
    Node proc_Statement(){
        Node Statement("Statement");
        if(is_Identifier()){
            Statement.add_son(proc_Assignment());
            if(get_word() == ";"){
                Statement.add_son(Node(";"));
                cur++;
            }
        }
        else if(is_Type()){
            Statement.add_son(proc_Declaration());
            if(get_word() == ";"){
                Statement.add_son(Node(";"));
                cur++;
            }
        }
        else if(get_word() == "if"){
            Statement.add_son(proc_IF());
        }
        else if(get_word() == "while"){
            Statement.add_son(proc_WHILE());
        }
        else if(get_word() == "break" || get_word() == "continue"){
            Statement.add_son(Node(get_word()));
            cur++;
            if(get_word() == ";"){
                Statement.add_son(Node(";"));
                cur++;
            }
        }
        return Statement;
    }
    
    Node proc_IF(){
        Node IF("IF");
        if(get_word() == "if"){
            IF.add_son(Node("if"));
            cur++;
            
            if(get_word()=="("){
                IF.add_son(Node("("));
                cur++;
            }
            IF.add_son(proc_Bool());
            if(get_word()==")"){
                IF.add_son(Node(")"));
                cur++;
            }
            IF.add_son(proc_Block());
            
            
            while(get_word() == "else if"){
                IF.add_son(proc_ELSE_IF());
            }
            
            
            if(get_word() == "else"){
                IF.add_son(proc_ELSE());
            }
        }
        return IF;
    }
    
    Node proc_WHILE(){
        Node WHILE("WHILE");
        
        WHILE.add_son(Node("while"));
        cur++;
        
        if(get_word()=="("){
            WHILE.add_son(Node("("));
            cur++;
        }
        WHILE.add_son(proc_Bool());
        if(get_word()==")"){
            WHILE.add_son(Node(")"));
            cur++;
        }
        WHILE.add_son(proc_Block());
        
        return WHILE;
    }
    
    Node proc_ELSE(){
        Node ELSE("ELSE");
        
        if(get_word() == "else"){
            ELSE.add_son(Node("else"));
            cur++;
        }

        ELSE.add_son(proc_Block());
        
        return ELSE;
    }
    
    Node proc_ELSE_IF(){
        Node ELSE_IF("ELSE_IF");
        if(get_word() == "else if"){
            ELSE_IF.add_son(Node("else if"));
            cur++;
        }
        if(get_word()=="("){
            ELSE_IF.add_son(Node("("));
            cur++;
        }
        ELSE_IF.add_son(proc_Bool());
        if(get_word()==")"){
            ELSE_IF.add_son(Node(")"));
            cur++;
        }
        ELSE_IF.add_son(proc_Block());
        return ELSE_IF;
    }
    
    Node proc_Declaration(){
        Node Declaration("Declaration");
        if(is_Type()){
            Node Type("Type");
            Type.add_son(Node(get_word()));
            Declaration.add_son(Type);
            cur++;
        }
        Declaration.add_son(proc_Descriptions());
        return Declaration;
        
    }
    
    Node proc_Descriptions(){
        Node Descriptions("Descriptions");
        Descriptions.add_son(proc_Description());
        while(get_word()==","){
            Descriptions.add_son(Node(","));
            cur++;
            Descriptions.add_son(proc_Description());
        }
        return Descriptions;
    }
    
    Node proc_Description(){
        Node Description("Description");
        if(is_Identifier() && get_next_word()=="="){
            Description.add_son(proc_Assignment());
        }
        else if(is_Identifier()){
            proc_Identifier();
        }
        else{
            ;
        }
        return Description;
    }
    
    Node proc_Identifier(){
        Node Identifier("Identifier");
        
        //create Node("a1") and add it to Node("Identifier")
        Identifier.add_son(Node(get_word()));
        cur++;
        return Identifier;
    }
    
    Node proc_Number(){
        Node Number("Number");
        Number.add_son(get_word());
        cur++;
        return Number;
    }
    
    Node proc_Assignment(){
        Node Assignment("Assignment");
        if(is_Identifier()){
            Assignment.add_son(proc_Identifier());
        }
        if(get_word() == "="){
            Assignment.add_son(Node("="));
            cur++;
        }

        Assignment.add_son(proc_Expression());
        return Assignment;
    }
    
    Node proc_Factor(){
        Node Factor("Factor");
        if(is_Identifier()){
            Factor.add_son(proc_Identifier());
        }
        else if(is_Number()){
            Factor.add_son(proc_Number());
        }
        else if(get_word()=="("){
            Factor.add_son(Node("("));
            cur++;
            
            Factor.add_son(proc_Expression());
            
            if(get_word()==")"){
                Factor.add_son(Node(")"));
                cur++;
            }
        }
        return Factor;
        
    }
    
    Node proc_Term(){
        Node Term("Term");
        Term.add_son(proc_Factor());
        while(get_word()=="*" || get_word()=="/"){
            Term.add_son(Node(get_word()));
            cur++;
            
            Term.add_son(proc_Factor());
        }
        return Term;
    }
    
    
    Node proc_Expression(){
        Node Expression("Expression");
        Expression.add_son(proc_Term());
        while(get_word()=="+" || get_word()=="-"){
            Expression.add_son(Node(get_word()));
            cur++;
            
            Expression.add_son(proc_Term());
        }
        return Expression;
        
    }
    
    Node proc_Bool(){
        Node Bool("Bool");
        
        Bool.add_son(proc_Expression());
        if(is_Compare_Op()){
            Bool.add_son(get_word());
            cur++;
            Bool.add_son(proc_Expression());
        }
        return Bool;
    }
};


#endif /* GrammarAnalyzer_h */
