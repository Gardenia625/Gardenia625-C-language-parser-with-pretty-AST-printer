#include "error.h"
#include "lexer.h"




int main(int argc, char* argv[]) {
    std::string fileNameWithDir;
    bool lexFlag = true;
    bool parFlag = false;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--lex") { 
            // Print tokens along with the position of their last character.
            lexFlag = true; continue;
        }
        if (arg == "--par") { parFlag = true; continue; }
        fileNameWithDir = arg;
    }
    Lexer lexer(fileNameWithDir, lexFlag);
    if (lexFlag) {
        std::cout << COLOR_TIP << "[ row: col] token" << COLOR_RESET << std::endl;
        while (true) {
            Token t = lexer.next();
            if (t.type == TokenType::END) { break; }
        }
    }
    error_summary(fileNameWithDir);
    return 0;
}