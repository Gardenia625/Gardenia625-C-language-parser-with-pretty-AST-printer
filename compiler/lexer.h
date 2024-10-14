#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

#include <format>

enum class TokenType;
TokenType getTokenType(const std::string&);

class Token {
public:
    Token(const std::string& s): name(s) {type = getTokenType(s);}
    void print() {std::cout << name << ' ';}
private:
    TokenType type;
    std::string name;
};


std::vector<Token> lexer(std::string);