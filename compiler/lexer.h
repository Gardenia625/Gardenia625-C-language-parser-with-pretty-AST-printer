#ifndef HEADER_LEXER
#define HEADER_LEXER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

#include <format>

#include "error.h"

using TokenValue = std::variant<int, char, string>;

enum class TokenType {
    ERROR,
    ENUM,
    COMMENT,
    // number
    INT,
    NUMBER,
    CHAR,    
    STRING,
    IDENTIFIER,
    TYPE,
    KEYWORD,
    // keyword
    RETURN,

    UNARY_OPERATOR,
    BINARY_OPERATOR,

    // symbols
    L_PARENTHESIS,
    R_PARENTHESIS,

    L_BRACE,
    R_BRACE,

    SEMICOLON,

    SYMBOL,
    END
};

struct Token {
    TokenType type;
    TokenValue value;
    int row;
    int col;
    void print();
};

class Lexer {
public:
    Lexer(string filename, bool flag);
    Token next();
private:
    std::ifstream file;
    char c;         // 下一个待读取的字符
    int row;        // c 对应的行
    int col;        // c 对应的列
    bool lex_flag;  // 是否打印
    void move_forward();
    Token next_token();
    Token next_identifier();
    Token next_number();
    Token next_char();
    Token next_string();
    Token next_comment();
    Token next_symbol();
};


inline Lexer::Lexer(string filename, bool flag)
    : row(0), col(-1), file(filename), lex_flag(flag) {
    if (!file) {
        cerr << "Failed to open the file." << endl;
        exit(1);
    }
    if (lex_flag) {
        cout << COLOR_TIP << "[ row: col] token" << COLOR_RESET << endl;
    }
    move_forward();
}

#endif