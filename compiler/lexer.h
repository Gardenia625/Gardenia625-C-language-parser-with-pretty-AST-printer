#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

#include <format>
#include <variant>

using TokenValue = std::variant<int, char, std::string>;

enum class TokenType {
    ERROR,
    COMMENT,
    NUMBER,
    CHAR,    
    STRING,
    IDENTIFIER,
    TYPE,
    KEYWORD,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
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
    // friend Parser;
    Lexer(std::string filename, bool flag);
    Token next();
private:
    std::ifstream file;
    char c;  // the next character waiting to be processed
    int row;
    int col;
    bool flag;
    void move_forward();
    Token next_token();
    Token next_identifier();
    Token next_number();
    Token next_char();
    Token next_string();
    Token next_comment();
    Token next_symbol();
};

// open the source file
inline Lexer::Lexer(std::string filename, bool flag)
    : row(0), col(-1), file(filename), flag(flag) {
    if (!file) {
        std::cerr << "Failed to open the file." << std::endl;
        exit(1);
    }
    move_forward();
}