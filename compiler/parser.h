

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
    void match(TokenType t);  // 确保当前 token 的类型为 t, 不是就报错
    bool is_operator(string s) { return token.is_operator(s); }
    unique_ptr<AST> global_declaration();
    unique_ptr<Statement> statement();
    unique_ptr<Block> block();
    CType specifier();
    string identifier();


    // void struct_declaration();
};