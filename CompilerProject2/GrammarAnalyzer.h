//
//  GrammarAnalyzer.h
//  CompilerProject2
//
//  Created by PatrickDD on 2021/4/18.
//

#include "Defitions.h"

#ifndef GrammarAnalyzer_h
#define GrammarAnalyzer_h

// Program -> int main() Block
// Block -> { Stmts }
// Stmts -> Stmt Stmts | Empty
// Stmt -> Asig_E ; | Decl ; | break ; | continue; | DO_WHILE | IF | WHILE | DO_WHILE ;
//                      GET ; | PUT ; | Asig_C ; | Asig_S ; | RETURN ;
// IF -> if ( Bool ) Block |
//       if ( Bool ) Block ELSE |
//       if ( Bool ) Block ELSE_IF ELSE
// ELSE_IF -> else if ( Bool ) Block | else if ( Bool ) Block ELSE_IF
// ELSE -> else Block
// WHILE -> while ( Bool ) Block
// DO_WHILE -> do Block while ( Bool ) ;
// RETURN -> return Expr
// Asig_C -> Id Asig_Op Expr
// Asig_S -> Id ++ | Id --
// Asig_E -> Id = Expr
// GET -> get ( Id )
// PUT -> put ( Expr )
// Decl -> Type Descs
// Descs -> Desc | Desc , Descs
// Desc -> Id | Asig_E
// Type -> bool | char | int | double | float | string
// Expr -> Expr + Term | Expr - Term | Term
// Term -> Term * Factor | Term / Factor | Factor
// Factor -> Number | Id | ( Expr )
// Bool -> Expr | Expr Op_Compare Expr
// Op_Compare -> == | >= | > | < | <=
// Number -> Decimal_Number | Octal_Number | Hexademical_Number


class Node{
public:
    string Component;
    vector<Node> sons;
    
    int id;
    
    static int cur_id;
    static const int format_length = 2;
    
    Node(string Component){
        this->Component = Component;
        id = ++cur_id;
    }
    
    void add_son(Node son){
        sons.push_back(son);
    }
    
    bool is_terminal(){
        return sons.size() == 0;
    }
    
    string format_Component(){
        string f_Component = Component;
        while(f_Component.length() < 4*format_length){
            f_Component += is_terminal()?" ":"-";
        }
        return f_Component;
    }
    
};

int Node::cur_id = 0;

class GrammarAnalyzer {
    
public:
    
    
    GrammarAnalyzer(vector<pair<vector<string>, int> > &lex_result):root("Root"){
        this->lex_result = lex_result;
    }
    
    void grammer_analysis(){
        Node Program = proc_Program();
        root.add_son(Program);
    }
    
    void draw_AST(){
        draw_Node(root, 0, 0);
    }
    
    void draw_Node(Node node, int cols_before, int index){
        if(index != 0){
            for(int i=0; i<cols_before; i++){
                for(int j=0; j<Node::format_length; j++){
                    printf("\t");
                }
            }
        }
        
        printf("%s", node.format_Component().c_str());
        if(node.is_terminal()){
            printf("\n");
        }
        else{
            for(int i=0; i<node.sons.size(); i++){
                draw_Node(node.sons[i], cols_before+1, i);
            }
        }
    }
    

private:
    
    int cur = 0;
    Node root;
    
    vector<pair<vector<string>, int> > lex_result;
    
    string get_word(){
//        int i = (cur == lex_result.size() ? cur : cur-1);
        return lex_result[cur].first[0];
    }
    
    string get_next_word(){
        return lex_result[cur+1].first[0];
    }
    
    void check_add(Node &node, string symbol){
        if(get_word() == symbol){
            node.add_son(Node(symbol));
            cur++;
        }
        else{
            
            node.add_son(Node("Error("+get_word()+")"));
            printf("[ERROR] Miss %s before '%s' [%s] - [%s]\n", symbol.c_str(), get_word().c_str(), lex_result[cur].first[1].c_str(), lex_result[cur].first[2].c_str());
        }
    }
    
    bool is_Type(){
        string word = get_word();
        return (word=="int" || word=="char" || word=="bool" ||
                word=="string" || word=="double" || word=="float");
    }
    
    bool is_Id(){
        int word_id = lex_result[cur].second;
        return KEY_WORDS[word_id-1] == "Id";
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
    
    bool is_Asig_C_Op(string word){
        return (word=="+=" || word=="-=" || word== "/=" ||
                word=="*=" || word=="^=" || word== "&=" ||
                word=="|=" || word=="%=" || word== "~=" || word=="!=");
    }
    
    bool is_Asig_C_Op(){
        return is_Asig_C_Op(get_word());
    }
    
    
    // Program -> int main Block
    Node proc_Program(){
        Node Program("Program");
        
        check_add(Program, "int");
        check_add(Program, "main");
        check_add(Program, "(");
        check_add(Program, ")");

        Program.add_son(proc_Block());

        return Program;
    }
    
    // Block -> { Stmts }
    Node proc_Block(){
        Node Block("Block");

        check_add(Block, "{");

        if(!(get_word() == "}")){
            Block.add_son(proc_Stmts());
        }
        
        check_add(Block, "}");
        return Block;
    }
    
    // Stmts -> Stmt Stmts | Empty
    Node proc_Stmts(){
        Node Stmts("Stmts");
        
        Stmts.add_son(proc_Stmt());

        while(!(get_word() == "}")){
            Stmts.add_son(proc_Stmt());
        }
            
        return Stmts;
    }
    
    
    //Stmt -> Asig ; | Decl ; | break ; | continue; | DO_WHILE | IF |
    //          WHILE | DO_WHILE ; | GET ; | PUT ; | Asig_C ; | Asig_S ; | RETURN ;
    Node proc_Stmt(){
        Node Stmt("Stmt");
        if(is_Id()){
            if(is_Asig_C_Op(get_next_word())){
                Stmt.add_son(proc_Asig_C());
                check_add(Stmt, ";");
            }
            else if(get_next_word()=="++" || get_next_word()=="--"){
                Stmt.add_son(proc_Asig_S());
                check_add(Stmt, ";");
            }
            else if(get_next_word()=="="){
                Stmt.add_son(proc_Asig_E());
                check_add(Stmt, ";");
            }
            else{
                Stmt.add_son(proc_Id());
                check_add(Stmt, "Assignment Operator");
                cur++;
            }
        }
        
        else if(is_Type()){
            Stmt.add_son(proc_Decl());
            check_add(Stmt, ";");
        }
        else if(get_word() == "if"){
            Stmt.add_son(proc_IF());
        }
        else if(get_word() == "while"){
            Stmt.add_son(proc_WHILE());
        }
        else if(get_word() == "do"){
            Stmt.add_son(proc_DO_WHILE());
            check_add(Stmt, ";");
        }
        else if(get_word() == "break" || get_word() == "continue"){
            check_add(Stmt, get_word());
            check_add(Stmt, ";");
        }
        
        else if(get_word() == "get"){
            Stmt.add_son(proc_GET());
            check_add(Stmt, ";");
        }
        else if(get_word() == "put"){
            Stmt.add_son(proc_PUT());
            check_add(Stmt, ";");
        }
        else if(get_word() == "return"){
            Stmt.add_son(proc_RETURN());
            check_add(Stmt, ";");
        }
        else{
            check_add(Stmt, "Legal Statement Word");
            cur++;
        }
        return Stmt;
    }
    
    // GET -> get ( Id )
    Node proc_GET(){
        Node GET("GET");
        
        check_add(GET, "get");
        check_add(GET, "(");
        
        if(is_Id()){
            GET.add_son(proc_Id());
        }
        
        check_add(GET, ")");
        return GET;
    }
    
    // PUT -> put ( Expr )
    Node proc_PUT(){
        Node PUT("PUT");
        
        check_add(PUT, "put");
        check_add(PUT, "(");
        
        PUT.add_son(proc_Expr());
        
        check_add(PUT, ")");
        
        return PUT;
    }
    
    // IF -> if ( Bool ) Block |
    //       if ( Bool ) Block else Block |
    //       if ( Bool ) Block ELSE_IF else Block
    Node proc_IF(){
        Node IF("IF");
        
        check_add(IF, "if");
        check_add(IF, "(");
        
        IF.add_son(proc_Bool());
        
        check_add(IF, ")");
        
        IF.add_son(proc_Block());
        
        
        while(get_word() == "else if"){
            IF.add_son(proc_ELSE_IF());
        }
        
        
        if(get_word() == "else"){
            IF.add_son(proc_ELSE());
        }
        return IF;
    }
    
    // WHILE -> while ( Bool ) Block
    Node proc_WHILE(){
        Node WHILE("WHILE");
        
        check_add(WHILE, "while");
        check_add(WHILE, "(");
        
        WHILE.add_son(proc_Bool());
        
        check_add(WHILE, ")");
        
        WHILE.add_son(proc_Block());
        
        return WHILE;
    }
    
    // DO_WHILE -> do Block while ( Bool )
    Node proc_DO_WHILE(){
        Node DO_WHILE("DO_WHILE");
        check_add(DO_WHILE, "do");
        DO_WHILE.add_son(proc_Block());
        check_add(DO_WHILE, "while");
        check_add(DO_WHILE, "(");
        DO_WHILE.add_son(proc_Bool());
        check_add(DO_WHILE, ")");
        return DO_WHILE;
    }
    
    // ELSE -> else Block
    Node proc_ELSE(){
        Node ELSE("ELSE");
        
        check_add(ELSE, "else");

        ELSE.add_son(proc_Block());
        
        return ELSE;
    }
    
    // ELSE_IF -> else if ( Bool ) Block | else if ( Bool ) Block ELSE_IF
    Node proc_ELSE_IF(){
        Node ELSE_IF("ELSE_IF");
        
        check_add(ELSE_IF, "else if");
        check_add(ELSE_IF, "(");
        
        ELSE_IF.add_son(proc_Bool());
        
        check_add(ELSE_IF, ")");
        
        ELSE_IF.add_son(proc_Block());
        return ELSE_IF;
    }
    
    Node proc_Type(){
        Node Type("Type");
        Type.add_son(Node(get_word()));
        cur++;
        return Type;
    }
    
    // Decl -> Type Descs
    Node proc_Decl(){
        Node Decl("Decl");
        
        if(is_Type()){
            Decl.add_son(proc_Type());
        }
        Decl.add_son(proc_Descs());
        
        return Decl;
        
    }
    
    // Descs -> Desc | Desc , Descs
    Node proc_Descs(){
        Node Descs("Descs");
        Descs.add_son(proc_Desc());
        while(get_word()==","){
            check_add(Descs, ",");
            Descs.add_son(proc_Desc());
        }
        return Descs;
    }
    
    // Desc -> Id | Asig_E
    Node proc_Desc(){
        Node Desc("Desc");
        if(is_Id() && get_next_word()=="="){
            Desc.add_son(proc_Asig_E());
        }
        else if(is_Id()){
            Desc.add_son(proc_Id());
        }
        return Desc;
    }
    
    Node proc_Id(){
        Node Id("Id");
        
        //create Node("a1") and add it to Node("Id")
        Id.add_son(Node(get_word()));
        cur++;
        return Id;
    }
    
    Node proc_Number(){
        Node Number("Number");
        Number.add_son(get_word());
        cur++;
        return Number;
    }
    
    // Asig_E -> Id = Expr
    Node proc_Asig_E(){
        Node Asig_E("Asig_E");
        
        if(is_Id()){
            Asig_E.add_son(proc_Id());
        }
        
        check_add(Asig_E, "=");

        Asig_E.add_son(proc_Expr());
        return Asig_E;
    }
    
    // Factor -> Number | Id | ( Expr )
    Node proc_Factor(){
        Node Factor("Factor");
        
        if(is_Id()){
            Factor.add_son(proc_Id());
        }
        else if(is_Number()){
            Factor.add_son(proc_Number());
        }
        
        else if(get_word()=="("){
            check_add(Factor, "(");
            
            Factor.add_son(proc_Expr());
            
            check_add(Factor, ")");
        }
        else{
            //error
            check_add(Factor, "Expression");
        }
        return Factor;
        
    }
    
    // Term -> Term * Factor | Term / Factor | Factor
    Node proc_Term(){
        Node Term("Term");
        
        Term.add_son(proc_Factor());
        
        while(get_word()=="*" || get_word()=="/"){
            check_add(Term, get_word());
            
            Term.add_son(proc_Factor());
        }
        return Term;
    }
    
    // Expr -> Expr + Term | Expr - Term | Term
    Node proc_Expr(){
        Node Expr("Expr");
        
        Expr.add_son(proc_Term());
        
        while(get_word()=="+" || get_word()=="-"){
            check_add(Expr, get_word());
            
            Expr.add_son(proc_Term());
        }
        return Expr;
        
    }
    
    // Bool -> Expr | Expr Op_Compare Expr
    // Op_Compare -> == | >= | > | < | <=
    Node proc_Bool(){
        Node Bool("Bool");
        
        Bool.add_son(proc_Expr());
        
        if(is_Compare_Op()){
            check_add(Bool, get_word());
            Bool.add_son(proc_Expr());
        }
        return Bool;
    }
    
    // RETURN -> return Expr
    Node proc_RETURN(){
        Node RETURN("RETURN");
        
        check_add(RETURN, "return");
        
        RETURN.add_son(proc_Expr());
        
        return RETURN;
    }
    
    // Asig_C -> Id Asig_Op Expr
    Node proc_Asig_C(){
        Node Asig_C("Asig_C");
        
        if(is_Id()){
            Asig_C.add_son(proc_Id());
        }
        if(is_Asig_C_Op()){
            check_add(Asig_C, get_word());
        }
        Asig_C.add_son(proc_Expr());
        
        return Asig_C;
    }
    
    // Asig_S -> Id ++ | Id --
    Node proc_Asig_S(){
        Node Asig_S("Asig_S");
        
        if(is_Id()){
            Asig_S.add_son(proc_Id());
        }
        if(get_word()=="++" || get_word()=="--"){
            check_add(Asig_S, get_word());
        }
        return Asig_S;
    }
};


#endif /* GrammarAnalyzer_h */
