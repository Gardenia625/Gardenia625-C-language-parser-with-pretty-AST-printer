#include "error.h"
#include "lexer.h"
#include "parser.h"



int main(int argc, char* argv[]) {
    string file_name_with_dir;
    bool lex_flag = true;
    bool par_flag = true;
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--lex") {         // print tokens
            lex_flag = true;
            continue;
        } else if (arg == "--par") {  // print the AST
            par_flag = true;
            continue;
        } else {
            file_name_with_dir = arg;
        }
    }
    Lexer lexer(file_name_with_dir, lex_flag);
    Parser parser(lexer, par_flag);
    parser.program();
    return 0;
}