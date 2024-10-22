#include "lexer.h"


void Token::print() {
    cout << "[" << std::format("{:>4}", std::to_string(row)) << ":"
         << std::format("{:>4}", std::to_string(col)) << "] ";
    switch (type) {
        case TT::CHAR:
            cout << "'" << value << "'";
            break;
        case TT::STRING:
            cout << "\"" << value << "\"";
            break;
        default:
            cout << value;
    }
    cout << endl;
}

std::unordered_map<string, TT> get_token_type {
    // type
    {"static", TT::STATIC},
    {"extern", TT::EXTERN},
    {"void", TT::VOID},
    {"char", TT::CHAR},
    {"int", TT::INT},
    {"long", TT::LONG},
    {"double", TT::DOUBLE},
    {"unsigned", TT::UNSIGNED},
    {"struct", TT::STRUCT},
    
    // {"enum", TokenType::ENUM},
    // {"float", TokenType::TYPE},
    // {"union", TokenType::TYPE},

    // keyword
    {"return", TT::RETURN},
    {"if", TT::IF},
    {"else", TT::ELSE},
    {"for", TT::FOR},
    {"while", TT::WHILE},
    {"do", TT::DO},
    {"continue", TT::CONTINUE},
    {"break", TT::BREAK},
    {"switch", TT::SWITCH},
    {"case", TT::CASE},
    {"default", TT::DEFAULT},

    
    {"sizeof", TT::OPERATOR},
    
    // {"include", TokenType::KEYWORD},
    // {"goto", TokenType::KEYWORD},
    // {"typedef", TokenType::KEYWORD},
    // {"__ignore_typecheck__", TokenType::KEYWORD},
    // {"const", TokenType::KEYWORD},
    
    // {"restrict", TokenType::KEYWORD},
};

std::unordered_map<char, char> get_escape_char {
    {'\\', '\\'},
    {'\'', '\''},
    {'"', '\"'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'b', '\b'},
    {'f', '\f'},
    {'v', '\v'},
    {'a', '\a'},
    {'0', '\0'}
};

// 读取文件的下一个字符
void Lexer::move_forward() {
    if (c == '\n') {
        ++row;
        col = 0;
    } else {
        ++col;
    }
    file.get(c);
}

// 获取下一个 token, 如有需要还会打印它
Token Lexer::next() {
    Token ret = next_token();
    if (lex_flag && ret.type != TT::END) {
        ret.print();
    }
    return ret;
}

Token Lexer::next_token() {
    while (!file.eof()) {
        if (isspace(c)) {              // 跳过空格
            move_forward();
            continue;
        }
        if (c == '/') {                // 可能是注释
            Token t = next_comment();
            if (t.type != TT::COMMENT) {
                return t;
            }
            continue;
        }
        if (isdigit(c)) {              // 数字
            return next_number();
        }
        if (isalpha(c) || c == '_') {  // 标识符 或 关键词
            return next_identifier();
        }
        if (c == '\'') {               // 字符
            return next_char();
        }
        if (c == '"') {                // 字符串
            return next_string();
        }
        return next_symbol();          // 运算符或其它符号
    }
    file.close();
    return Token(TT::END, "", row, col);
}

Token Lexer::next_comment() {
    Token ret(TT::COMMENT, "/", row, col);
    bool maybe_end = false;
    move_forward();
    switch(c) {
        case '/':           // 单行注释
            while (c != '\n' && !file.eof()) {
                move_forward();
            }
            return ret;
        case '*':           // 多行注释
            while (true) {  // 只能以 "*/" 为结尾
                move_forward();
                if (file.eof()) {
                    lexer_error("unterminated comment", ret.row);
                } else if (maybe_end || c == '/'){
                    move_forward();
                    return ret;
                }
                maybe_end = (c == '*');
            }
        default:            // 运算符 "/" 或 "/="
            ret.type = TT::OPERATOR;
            if (c == '=') {
                ret.value = "/=";
                move_forward();
            }
            return ret;
    }
}

std::unordered_set<char> end_of_number {
    '+', '-', '*', '/', '%', '&', '|', '^', '<', '=', '>', '!',
    ')', ']', '}', ',', ';'
};

Token Lexer::next_number() {
    Token ret(TT::NUMBER, "", row, col);
    bool is_float = false;
    // if (c == '.' || c == 'e' || c == 'E' || c == 'p' || c == 'P') {
    //     is_float == true;
    // }
    // if (c == '0') {
    //     move_forward();
    //     if (c == 'x' || c == 'X') {  // base 16

    //     } else {                     // base 8
            
    //     }
    // } else {                         // base 10

    // }

    while (isdigit(c)) {
        ret.value += c;
        move_forward();
    }
    auto it = end_of_number.find(c);
    if (it == end_of_number.end()) {
        lexer_error("invalid number", ret.row);
        do {
            move_forward();
            it = end_of_number.find(c);
        } while (it == end_of_number.end() && !file.eof());
    }
    return ret;
}

Token Lexer::next_identifier() {
    Token ret(TT::IDENTIFIER, "", row, col);
    while((isalnum(c) || c == '_') && !file.eof()) {
        ret.value += c;
        move_forward();
    }
    auto it = get_token_type.find(ret.value);
    if (it != get_token_type.end()) {  // 关键词
        ret.type = it->second;
    }
    return ret;
}

Token Lexer::next_char() {
    Token ret(TT::CHAR, "", row, col);
    move_forward();
    ret.value = c;
    if (c == '\\') {  // 转义符号
        move_forward();
        auto it = get_escape_char.find(c);
        if (it != get_escape_char.end()) {
            ret.value = it->second;
        } else {
            lexer_error(std::format("unknown escape sequence '\\{}'", c), row);
        }
    }
    move_forward();
    if (c != '\'') {
        lexer_error("missing terminating ' character", row);
    }
    move_forward();
    return ret;
}

Token Lexer::next_string() {
    Token ret(TT::STRING, "", row, col);
    move_forward();
    string s;
    while (c != '"') {
        if (c == '\\') {             // 转义字符
            move_forward();
            if (file.eof()) {
                lexer_error("missing terminating \" character", ret.row);
            } else if (c == '\n') {  // 续行符
                move_forward();
                continue;
            }
            auto it = get_escape_char.find(c);
            if (it != get_escape_char.end()) {
                s += it->second;
            } else {
                lexer_error(std::format("unknown escape sequence '\\{}'", c), row);
            }
        } else {
            s += c;
        } 
        move_forward();
        if (file.eof()) {
            lexer_error("missing terminating \" character", row);
        }
    }
    ret.value = s;
    move_forward();
    return ret;
}

Token Lexer::next_symbol() {
    Token ret(TT::OPERATOR, string{c}, row, col);
    bool unary = true;
    char first = c;
    move_forward();
    char second = c;
    switch (first) {
        // 运算符
        case '+':
        case '*':
        case '%':
        case '&':
        case '|':
        case '^':
        case '<':
        case '>':
        case '=':
            if (first == second || second == '=') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        case '!':
            if (second == '=') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        
        case '-':
            if (second == '-' || second == '=' || second == '>') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        case '~':
        case '.':
        case '?':
        case ':':
            break;
        // 符号
        case '(': { ret.type = TT::L_PARENTHESIS; break; }
        case ')': { ret.type = TT::R_PARENTHESIS; break; }
        case '[': { ret.type = TT::L_BRACKET; break; }
        case ']': { ret.type = TT::R_BRACKET; break; }
        case '{': { ret.type = TT::L_BRACE; break; }
        case '}': { ret.type = TT::R_BRACE; break; }
        case ',': { ret.type = TT::COMMA; break; }
        case ';': { ret.type = TT::SEMICOLON; break; }
        case '#': { ret.type = TT::HASH; break; }
        case '\\':
            move_forward();
            if (c == '\n') {  // 续行符
                move_forward();
                return next_token();
            } else {
                lexer_error("stray '\\'", ret.row);
            }
        default:
            lexer_error(std::format("unknown symbol '{}'", first), ret.row);
    }
    if (!unary) {
        move_forward();
    }
    return ret;
}