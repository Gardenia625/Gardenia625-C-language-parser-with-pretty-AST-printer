#include "error.h"
#include "lexer.h"




int main(int argc, char* argv[]) {
    std::string fileNameWithDir;
    bool lexFlag = true;  // print tokens
    bool parFlag = false;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--lex") { lexFlag = true; continue; }
        if (arg == "--par") { parFlag = true; continue; }
        fileNameWithDir = arg;
    }
    Lexer lexer(fileNameWithDir, lexFlag);
    if (lexFlag) {
        while (true) {
            Token t = lexer.next();
            if (t.type == TokenType::END) { break; }
        }
    }
    error_summary(fileNameWithDir);
    return 0;
}