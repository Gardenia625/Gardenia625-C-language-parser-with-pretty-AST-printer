#include "error.h"
#include "lexer.h"
#include "parser.h"



int main(int argc, char* argv[]) {
    string file_name_with_dir;
    bool lex_flag = true;
    bool par_flag = true;
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--lex") { 
            // 打印 token
            lex_flag = true;
            continue;
        } else if (arg == "--lex") {
            // 打印 AST
            par_flag = true;
            continue;
        }
        if (arg == "--par") { par_flag = true; continue; }
        file_name_with_dir = arg;
    }
    Lexer lexer(file_name_with_dir, lex_flag);
    Parser parser(lexer, par_flag);
    parser.program();
    return 0;
}