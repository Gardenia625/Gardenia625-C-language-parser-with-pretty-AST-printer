#include "error.h"


// 打印报错信息并终止编译
void print_error(std::string_view stage, std::string_view message, int line) {
    cerr << COLOR_ERROR << std::format("error at line {}: ", line) << COLOR_RESET 
         << message << endl
         << "compilation failed: terminates at the " << stage << " stage" << endl;
    exit(1);
}

void lexer_error(std::string_view message, int line) {
    print_error("lexing", message, line);
}

void parser_error(std::string_view message, int line) {
    print_error("parsing", message, line);
}