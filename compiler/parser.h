

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
    Token token;    // 下一个等待使用的 token
    bool par_flag;  // 是否打印 AST
    Token next() {
        Token ret = token;
        token = lexer.next();
        return ret;
    };
    unique_ptr<AST> global_declaration();
    unique_ptr<Statement> statement();
    unique_ptr<Block> block();
    CType specifier();
    string identifier();
};