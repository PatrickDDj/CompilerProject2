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
// Block -> { Stmts }
// Stmts -> Stmt Stmts | Empty
// Stmt -> Asig ; | Decl ; | break ; | continue; | DO_WHILE | IF | WHILE |
//                      GET ; | PUT ; | Asig_C ; | Asig_S ; | RETURN ;
// IF -> if ( Bool ) Block |
//       if ( Bool ) Block else Block |
//       if ( Bool ) Block ELSE_IF else Block
// ELSE_IF -> else if ( Bool ) Block ELSE_IF | Empty
// RETURN -> return Expr
// Asig_C -> Id Asig_Op Expr
// Asig_S -> Id ++ | Id --
// GET -> get ( Id )
// PUT -> put ( Expr )
// Asig -> Id = Expr
// Decl -> Type Descs
// Descs -> Desc | Desc , Descs
// Desc -> Id | Asig
// Type -> bool | char | int | double | float | string
// Expr -> Expr + Term | Expr - Term | Term
// Term -> Term * Factor | Term / Factor | Factor
// Factor -> Number | Id | ( Expr )
// Bool -> Expr | Expr Op Expr
// Op -> == | >= | > | < | <=
// Number -> Decimal_Number | Octal_Number | Hexademical_Number


class Node{
public:
    string Component;
    vector<Node> sons;
    
    int id;
    
    static int cur_id;
    
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
        while(f_Component.length() < 4*node_format_length){
            f_Component += is_terminal()?" ":"-";
        }
        return f_Component;
    }
    
};

int Node::cur_id = 0;


class GrammarAnalyzer {
    
public:
    
    
    GrammarAnalyzer(string code_path):root("Root"){
        LexicalAnalyzer lex(code_path);
        lex_result = lex.lex_result;
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
                for(int j=0; j<node_format_length; j++){
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
    
    void print_AST(){
        print_Node(root);
    }
    
    void print_Node(Node node){
        printf("Node : %s\n", node.Component.c_str());
        for(Node son : node.sons){
            print_Node(son);
        }
    }
private:
    
    int cur = 0;
    Node root;
    vector<pair<vector<string>, int> > lex_result;
    
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
    
    
    
    Node proc_Program(){
        Node Program("Program");
        
        if(get_word() == "int"){
            cur++;
            Program.add_son(Node("int"));
        }
        if(get_word() == "main"){
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
            Block.add_son(proc_Stmts());
        }
        
        if(get_word() == "}"){
            Block.add_son(Node("}"));
            cur++;
        }
        return Block;
    }
    
    Node proc_Stmts(){
        Node Stmts("Stmts");
        
        Stmts.add_son(proc_Stmt());

        while(!(get_word() == "}")){
            Stmts.add_son(proc_Stmt());
        }
            
        return Stmts;
    }
    
    Node proc_Stmt(){
        Node Stmt("Stmt");
        if(is_Id()){
            if(is_Asig_C_Op(get_next_word())){
                Stmt.add_son(proc_Asig_C());
                if(get_word() == ";"){
                    Stmt.add_son(Node(";"));
                    cur++;
                }
            }
            else if(get_next_word()=="++" || get_next_word()=="--"){
                Stmt.add_son(proc_Asig_S());
                if(get_word() == ";"){
                    Stmt.add_son(Node(";"));
                    cur++;
                }
            }
            else if(get_next_word()=="="){
                Stmt.add_son(proc_Asig_E());
                if(get_word() == ";"){
                    Stmt.add_son(Node(";"));
                    cur++;
                }
            }
        }
        
        else if(is_Type()){
            Stmt.add_son(proc_Decl());
            if(get_word() == ";"){
                Stmt.add_son(Node(";"));
                cur++;
            }
        }
        else if(get_word() == "if"){
            Stmt.add_son(proc_IF());
        }
        else if(get_word() == "while"){
            Stmt.add_son(proc_WHILE());
        }
        else if(get_word() == "break" || get_word() == "continue"){
            Stmt.add_son(Node(get_word()));
            cur++;
            if(get_word() == ";"){
                Stmt.add_son(Node(";"));
                cur++;
            }
        }
        
        else if(get_word() == "get"){
            Stmt.add_son(proc_GET());
            if(get_word() == ";"){
                Stmt.add_son(Node(";"));
                cur++;
            }
        }
        else if(get_word() == "put"){
            Stmt.add_son(proc_PUT());
            if(get_word() == ";"){
                Stmt.add_son(Node(";"));
                cur++;
            }
        }
        else if(get_word() == "return"){
            Stmt.add_son(proc_RETURN());
            if(get_word() == ";"){
                Stmt.add_son(Node(";"));
                cur++;
            }
        }
        return Stmt;
    }
    
    Node proc_GET(){
        Node GET("GET");
        if(get_word() == "get"){
            GET.add_son(Node("get"));
            cur++;
        }
        if(get_word() == "("){
            GET.add_son(Node("("));
            cur++;
        }
        if(is_Id()){
            GET.add_son(proc_Id());
        }
        if(get_word() == ")"){
            GET.add_son(Node(")"));
            cur++;
        }
        return GET;
    }
    
    Node proc_PUT(){
        Node PUT("PUT");
        if(get_word() == "put"){
            PUT.add_son(Node("put"));
            cur++;
        }
        if(get_word() == "("){
            PUT.add_son(Node("("));
            cur++;
        }
        PUT.add_son(proc_Expr());
        if(get_word() == ")"){
            PUT.add_son(Node(")"));
            cur++;
        }
        return PUT;
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
    
    Node proc_Decl(){
        Node Decl("Decl");
        if(is_Type()){
            Node Type("Type");
            Type.add_son(Node(get_word()));
            Decl.add_son(Type);
            cur++;
        }
        Decl.add_son(proc_Descs());
        return Decl;
        
    }
    
    Node proc_Descs(){
        Node Descs("Descs");
        Descs.add_son(proc_Desc());
        while(get_word()==","){
            Descs.add_son(Node(","));
            cur++;
            Descs.add_son(proc_Desc());
        }
        return Descs;
    }
    
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
    
    Node proc_Asig_E(){
        Node Asig_E("Asig_E");
        if(is_Id()){
            Asig_E.add_son(proc_Id());
        }
        if(get_word() == "="){
            Asig_E.add_son(Node("="));
            cur++;
        }

        Asig_E.add_son(proc_Expr());
        return Asig_E;
    }
    
    Node proc_Factor(){
        Node Factor("Factor");
        if(is_Id()){
            Factor.add_son(proc_Id());
        }
        else if(is_Number()){
            Factor.add_son(proc_Number());
        }
        else if(get_word()=="("){
            Factor.add_son(Node("("));
            cur++;
            
            Factor.add_son(proc_Expr());
            
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
    
    
    Node proc_Expr(){
        Node Expr("Expr");
        Expr.add_son(proc_Term());
        while(get_word()=="+" || get_word()=="-"){
            Expr.add_son(Node(get_word()));
            cur++;
            
            Expr.add_son(proc_Term());
        }
        return Expr;
        
    }
    
    Node proc_Bool(){
        Node Bool("Bool");
        
        Bool.add_son(proc_Expr());
        if(is_Compare_Op()){
            Bool.add_son(get_word());
            cur++;
            Bool.add_son(proc_Expr());
        }
        return Bool;
    }
    
    // RETURN -> return Expr
    Node proc_RETURN(){
        Node RETURN("RETURN");
        if(get_word() == "return"){
            RETURN.add_son(Node(get_word()));
            cur++;
        }
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
            Asig_C.add_son(Node(get_word()));
            cur++;
        }
        Asig_C.add_son(proc_Expr());
        return Asig_C;
    }
    
    Node proc_Asig_S(){
        Node Asig_S("Asig_S");
        if(is_Id()){
            Asig_S.add_son(proc_Id());
        }
        if(get_word()=="++" || get_next_word()=="--"){
            Asig_S.add_son(Node(get_word()));
            cur++;
        }
        return Asig_S;
    }
};


#endif /* GrammarAnalyzer_h */
