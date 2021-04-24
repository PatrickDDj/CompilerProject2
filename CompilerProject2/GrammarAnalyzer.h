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

// IF -> if ( Expr ) Block |
//       if ( Expr ) Block ELSE |
//       if ( Expr ) Block ELSE_IF ELSE
// ELSE_IF -> else if ( Expr ) Block | else if ( Expr ) Block ELSE_IF
// ELSE -> else Block

// WHILE -> while ( Expr ) Block
// DO_WHILE -> do Block while ( Expr ) ;

// SWITCH -> switch ( Expr ) { CASES DEFAULT }
// CASES -> CASE | CASE CASES
// CASE -> case constant : Stmts
// DEFAULT -> default : Stmts

// RETURN -> return Expr

// Asig_C -> Id Asig_Op Expr
// Asig_S -> Id ++ | Id --
// Asig_E -> Id = Expr

// F_Call -> F_Name ( Paras )
// Paras -> Para | Para , Paras | Empty
// Para -> Expr

// Decl -> Type Descs
// Descs -> Desc | Desc , Descs
// Desc -> Id | Asig_E
// Type -> bool | char | int | double | float | string

// Expr -> Factor | Factor BinOp Expr
// Factor -> Number | ( Expr ) | Id | F_Call
// BinOp -> == | >= | > | < | <= | + ...
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
        grammer_analysis();
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
    
    bool is_TF(){
        string word = get_word();
        return (word=="true" || word=="false");
    }
    
    bool is_Constant(){
        return is_Number() || is_TF();
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

        while(!(get_word() == "}" || get_word()=="case" || get_word()=="default")){
            Stmts.add_son(proc_Stmt());
        }
            
        return Stmts;
    }
    
    
    //Stmt -> Asig_E ; | Decl ; | break ; | continue; | DO_WHILE | IF |
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
            //Function Call
            else if(get_next_word()=="("){
                Stmt.add_son(proc_Id());
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
        else if(get_word()=="switch"){
            Stmt.add_son(proc_SWITCH());
        }
        
//        else if(get_word() == "get"){
//            Stmt.add_son(proc_GET());
//            check_add(Stmt, ";");
//        }
//        else if(get_word() == "put"){
//            Stmt.add_son(proc_PUT());
//            check_add(Stmt, ";");
//        }
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
//    Node proc_GET(){
//        Node GET("GET");
//
//        check_add(GET, "get");
//        check_add(GET, "(");
//
//        if(is_Id()){
//            GET.add_son(proc_Id());
//        }
//
//        check_add(GET, ")");
//        return GET;
//    }
    
    // PUT -> put ( Expr )
//    Node proc_PUT(){
//        Node PUT("PUT");
//
//        check_add(PUT, "put");
//        check_add(PUT, "(");
//
//        PUT.add_son(proc_Expr());
//
//        check_add(PUT, ")");
//
//        return PUT;
//    }
    
    // IF -> if ( Expr ) Block |
    //       if ( Expr ) Block else Block |
    //       if ( Expr ) Block ELSE_IF else Block
    Node proc_IF(){
        Node IF("IF");
        
        check_add(IF, "if");
        check_add(IF, "(");
        
        IF.add_son(proc_Expr());
        
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
    
    // WHILE -> while ( Expr ) Block
    Node proc_WHILE(){
        Node WHILE("WHILE");
        
        check_add(WHILE, "while");
        check_add(WHILE, "(");
        
        WHILE.add_son(proc_Expr());
        
        check_add(WHILE, ")");
        
        WHILE.add_son(proc_Block());
        
        return WHILE;
    }
    
    // DO_WHILE -> do Block while ( Expr )
    Node proc_DO_WHILE(){
        Node DO_WHILE("DO_WHILE");
        check_add(DO_WHILE, "do");
        DO_WHILE.add_son(proc_Block());
        check_add(DO_WHILE, "while");
        check_add(DO_WHILE, "(");
        DO_WHILE.add_son(proc_Expr());
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
    
    // ELSE_IF -> else if ( Expr ) Block | else if ( Expr ) Block ELSE_IF
    Node proc_ELSE_IF(){
        Node ELSE_IF("ELSE_IF");
        
        check_add(ELSE_IF, "else if");
        check_add(ELSE_IF, "(");
        
        ELSE_IF.add_son(proc_Expr());
        
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
    
    Node proc_Paras(){
        Node Paras("Paras");
        while(get_word()!=")"){
            Node Para("Para");
            Para.add_son(proc_Expr());
            Paras.add_son(Para);
            if(get_word()==","){
                check_add(Paras, ",");
            }
        }
        return Paras;
    }
    
    Node proc_F_Name(){
        Node F_Name("F_Name");
        check_add(F_Name, get_word());
        return F_Name;
    }
    
    
    //Id or F_Call
    Node proc_Id(){
        if(get_next_word()=="("){
            Node F_Call("F_Call");
            
            F_Call.add_son(proc_F_Name());
            check_add(F_Call, "(");
            
            F_Call.add_son(proc_Paras());
            check_add(F_Call, ")");
            return F_Call;
        }
        else{
            Node Id("Id");
            
            //create Node("a1") and add it to Node("Id")
            Id.add_son(Node(get_word()));
            cur++;
            return Id;
        }
    }
    
    Node proc_Number(){
        Node Number("Number");
        Number.add_son(get_word());
        cur++;
        return Number;
    }
    
    Node proc_TF(){
        Node TF("TF");
        TF.add_son(Node(get_word()));
        cur++;
        return TF;
    }
    
    Node proc_Constant(){
        Node Constant("Constant");
        if(is_Number()){
            Constant.add_son(proc_Number());
        }
        else if(is_TF()){
            Constant.add_son(proc_TF());
        }
        return Constant;
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
        
        else if(is_TF()){
            Factor.add_son(proc_TF());
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

    
    int get_Prec(string op){
        return Op_Precedence[op];
    }
    
    Node merge(Node &LHS, Node &op, Node &RHS){
        Node c("Expr");
        c.add_son(LHS);
        c.add_son(op);
        c.add_son(RHS);
        return c;
    }
    
    Node proc_Expr(){
        Node LHS = proc_Factor();
        return proc_RHS(LHS, 0);
    }
    
    Node proc_RHS(Node &LHS, int Expr_Prec){
        while(true){
            string op = get_word();
            int cur_Prec = Op_Precedence[op];
            
            
            if(cur_Prec <= Expr_Prec){
                return LHS;
            }
            cur++;
            Node RHS = proc_Factor();
            
            string next_op = get_word();
            
            int next_Prec = Op_Precedence[next_op];
            
            if(next_Prec > cur_Prec){
                RHS = proc_RHS(RHS, cur_Prec);
            }
            Node Op = Node(op);
            LHS = merge(LHS, Op, RHS);
        }
    }
    

    
    // SWITCH -> switch ( Expr ) { CASES DEFAULT }
    Node proc_SWITCH(){
        Node SWITCH("SWITCH");
        
        check_add(SWITCH, "switch");
        check_add(SWITCH, "(");
        SWITCH.add_son(proc_Expr());
        check_add(SWITCH, ")");
        
        check_add(SWITCH, "{");
        SWITCH.add_son(proc_CASES());
        SWITCH.add_son(proc_DEFAULT());
        
        check_add(SWITCH, "}");
        
        return SWITCH;
    }
    
    // CASES -> CASE | CASE CASES
    Node proc_CASES(){
        Node CASES("CASES");
        
        while(get_word()=="case"){
            CASES.add_son(proc_CASE());
        }
        
        return CASES;
    }
    
    
    // CASE -> case constant : Stmts
    Node proc_CASE(){
        Node CASE("CASE");
        
        check_add(CASE, "case");
        if(is_Constant()){
            CASE.add_son(proc_Constant());
        }
        check_add(CASE, ":");
        CASE.add_son(proc_Stmts());
        
        
        return CASE;
    }
    
    
    // DEFAULT -> default : Stmts
    Node proc_DEFAULT(){
        Node DEFAULT("DEFAULT");
        check_add(DEFAULT, "default");
        check_add(DEFAULT, ":");
        if(get_word()!="}"){
            DEFAULT.add_son(proc_Stmts());
        }
        return DEFAULT;
    }
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
