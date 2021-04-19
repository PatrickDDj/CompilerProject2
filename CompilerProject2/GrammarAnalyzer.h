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
// Statement -> Assignment ; | Declaration ; |
//              if ( Bool ) Block |
//              if ( Bool ) Block else Block |
//              while ( Bool ) Block | break ;
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


//Termial Symbol : Identifier |

class Node{
public:
    string Component;
//    bool is_terminal;
//    int id;
    vector<Node> sons;
    
    Node(string Component){
        this->Component = Component;
//        this->is_terminal = is_terminal;
    }
    
    void add_son(Node son){
        sons.push_back(son);
    }

    void add_sons(vector<Node> sons){
        this->sons = sons;
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
    
//    pair<string, int> get_word(){
//        return make_pair(lex_result[cur].first[0], lex_result[cur].second);
//    }
    
    string get_word(){
        return lex_result[cur].first[0];
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
        return (word_type=="Octal_Number") || (word_type=="Decimal_Number") || (word_type=="Hexademical_Number");
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
//        Node Statements = proc_Statements();
//        if(!Statements.is_Empty()){
//            Block.add_son(Statements);
//        }
//
        Block.add_son(proc_Statements());
        if(get_word() == "}"){
            Block.add_son(Node("}"));
            cur++;
        }
        return Block;
    }
    
    Node proc_Statements(){
        //Statements -> Empty
        if(get_word() == "}"){
            return Node("Empty");
        }
        else{
            Node Statements("Statements");
            
            Statements.add_son(proc_Statement());
            
//            Node Statements_t = proc_Statements();
//
//            if(!Statements_t.is_Empty()){
//                Statements.add_son(Statements_t);
//            }
            Statements.add_son(proc_Statements());
            
            return Statements;
        }
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
        return Statement;
    }
    
    Node proc_Assignment(){
        Node Assignment("Assignment");
        if(is_Identifier()){
            Node Identifier("Identifier");
            
            //create Node("a1") and add it to Node("Identifier")
            Identifier.add_son(Node(get_word()));
            Assignment.add_son(Identifier);
            cur++;
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
            Node Identifier("Identifier");
            Identifier.add_son(get_word());
            Factor.add_son(Identifier);
            cur++;
        }
        else if(is_Number()){
            Node Number("Number");
            Number.add_son(get_word());
            Factor.add_son(Number);
            cur++;
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
    
    
};


#endif /* GrammarAnalyzer_h */
