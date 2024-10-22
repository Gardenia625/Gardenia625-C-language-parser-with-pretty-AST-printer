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
    Token token;    // peek
    bool par_flag;  // 是否打印 AST
    Token consume() {
        Token ret = token;
        token = lexer.next();
        return ret;
    };
    void match(TT t);  // 确保当前 token 的类型为 t, 不是就报错
    bool is_operator(string s) { return token.is_operator(s); }
    bool is_specifier() { return token.is_specifier(); }
    unique_ptr<AST> declaration(bool global);
    CType specifier(bool global);
    CType type_specifier();
    CDecl declarator();
    vector<unique_ptr<Parameter>> parameter_list();
    unique_ptr<Parameter> parameter();
    unique_ptr<Initializer> initializer();

    unique_ptr<Statement> statement();
    unique_ptr<Block> block();


    unique_ptr<Expression> expression();

    string identifier();


    // void struct_declaration();
};