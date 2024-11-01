#include "error.h"
#include "lexer.h"
#include "AST.h"



class Parser {
public:
    Parser(Lexer& l, bool flag): lexer(l), par_flag(flag) {
        token = lexer.next();
    }
    unique_ptr<Program> program();
private:
    Lexer& lexer;
    Token token;    // current token, i.e., the next token to be used
    bool par_flag;  // whether to print the AST
    Token consume() {
        Token ret = token;
        token = lexer.next();
        return ret;
    };
    void match(TT t);
    bool is_specifier() { return token.is_specifier(); }
    bool is_operator(string s="") { return token.is_operator(s); }
    bool is_unary();
    bool is_binary();
    unique_ptr<AST> declaration(bool global);
    CType specifier(bool global);
    CType type_specifier();
    Declarator declarator();
    vector<Parameter> parameter_list();
    Parameter parameter();
    unique_ptr<Initializer> initializer();

    unique_ptr<Statement> statement();
    unique_ptr<Block> block();


    unique_ptr<Expression> expression(int min_prec=0);
    unique_ptr<Expression> factor();
    vector<unique_ptr<Expression>> argument_list();
    string identifier();


    // void struct_declaration();
};