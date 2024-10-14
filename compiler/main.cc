#include "lexer.h"




int main(int argc, char* argv[]) {
    std::string dir;
    bool lex_flag = false;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--lex") {lex_flag = true; continue;}
        dir = arg;
    }
    auto tokens = lexer(dir);
    if (lex_flag) {
        for (Token token : tokens) token.print();
        std::cout << std::endl;
        return 0;
    }
}