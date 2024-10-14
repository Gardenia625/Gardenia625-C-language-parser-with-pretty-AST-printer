#include "lexer.h"

enum class TokenType {
        IDENTIFIER,
        CONSTANT,
        VOID,
        INT,
        RETURN,
        L_PARENTHESIS,
        R_PARENTHESIS,
        L_BRACE,
        R_BRACE,
        SEMICOLON,
};

const std::unordered_map<std::string, TokenType> tokenTypeMap {
    // built-in types
    {"void", TokenType::VOID},
    {"int", TokenType::INT},
    // keywords
    {"return", TokenType::RETURN},
    // single-symbol tokens
    {"(", TokenType::L_PARENTHESIS},
    {")", TokenType::R_PARENTHESIS},
    {"{", TokenType::L_BRACE},
    {"}", TokenType::R_BRACE},
    {";", TokenType::SEMICOLON},
  
};

TokenType getTokenType(const std::string& s) {
    auto it = tokenTypeMap.find(s);
    if (it != tokenTypeMap.end()) {
        return it->second;
    }
    if (isdigit(s[0])) {
        return TokenType::CONSTANT;
    }
    return TokenType::IDENTIFIER;
}



// single-symbol tokens
std::unordered_set<char> symbols{'(', ')', '{', '}', ';'};

// print the error message and terminate the program
void tokenError(const std::string& message, int lineNumber, const std::string& line) {
    std::cerr << "\033[1;91merror: \033[0m" << message << std::endl
              << std::format("{:>5}", std::to_string(lineNumber))
              << "  | " << line << "       |"  << std::endl;
    exit(1);
}

enum class State { 
    nothing,
    number,
    name,
    maybe_comment,
    single_comment,
    multi_comment,
    comment_end,
};

std::vector<Token> lexer(std::string filename) {
    std::vector<Token> tokens;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open the file." << std::endl;
        exit(1);
    }

    State state = State::nothing;
    int lineNumber = -1;
    std::string line;
    char c;
    std::string name;
    while (getline(file, line)) {
        ++lineNumber;
        line.push_back('\n');
        std::istringstream iss(line);
        while (iss.get(c)) {
            if (isspace(c) || symbols.find(c) != symbols.end()) {
                if (!name.empty()) {
                    tokens.emplace_back(name);
                    state = State::nothing;
                    name.clear();
                }
                if (!isspace(c)) {
                    tokens.emplace_back(std::string{c});
                }
                continue;
            }
            switch (state) {
                case State::nothing:
                    if (c == '/') {
                        state = State::maybe_comment;
                        break; 
                    }
                    name.push_back(c);
                    if (isdigit(c)) {
                        state = State::number;
                        break;
                    }
                    if (isalpha(c) || c == '_') {
                        state = State::name;
                        break;
                    }
                    tokenError(std::format("invalid token '{}'", c), lineNumber, line);
                case State::number:
                    if (isdigit(c)) {
                        name.push_back(c);
                        break;
                    }
                    tokenError(std::format("invalid character '{}' in number", c), lineNumber, line);
                case State::name:
                    if (isalnum(c) || c == '_') {
                        name.push_back(c);
                        break;
                    }
                    tokenError(std::format("invalid character '{}' in identifier", c), lineNumber, line);
                case State::maybe_comment:
                    if (c == '/') {
                        state = State::single_comment;
                        break;
                    }
                    if (c == '*') {
                        state = State::multi_comment;
                        break;
                    }
                    tokenError("invalid character '/'", lineNumber, line);
                case State::multi_comment:
                    if (c == '*') {
                        state = State::comment_end;
                    }
                    break;
                case State::comment_end:
                    if (c == '/') {
                        state = State::nothing;
                    }
                    else {
                        state = State::multi_comment;
                    }
                    break;
            }

            if (state == State::single_comment) { state = State::nothing; break; }
        }
    }

    file.close();
    return tokens;
}
