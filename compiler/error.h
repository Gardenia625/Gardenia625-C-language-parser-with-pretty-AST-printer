#ifndef HEADER_ERROR
#define HEADER_ERROR

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <set>


// 所有文件都将包含这个头文件, 故将这些代码放在这里
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

using std::vector;
using std::string;

// ANSI 颜色
#define COLOR_ERROR "\033[1;91m"  // bold high intensity red
#define COLOR_TIP "\033[1;96m"    // bold high intensity cyan
#define COLOR_RESET "\033[0m"



// 遇到错误直接终止编译
void lexer_error(std::string_view message, int line);
void parser_error(std::string_view message, int line);

#endif