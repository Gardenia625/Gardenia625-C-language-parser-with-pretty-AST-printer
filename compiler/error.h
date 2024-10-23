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
// 1 红
// 2 绿
// 3 黄 (亮)
// 4 蓝
// 5 紫
// 6 青
#define COLOR_ERROR "\033[1;91m"  // bold high intensity red
#define COLOR_TITLE "\033[1;93m"    //
#define COLOR_CLASS "\033[1;36m"  // 
#define COLOR_COMPONENT "\033[0;36m"  //
#define COLOR_TYPE "\033[0;34m"
#define COLOR_KEYWORD "\033[0;35m"  //
#define COLOR_NORMAL "\033[0;32m"
#define COLOR_RESET "\033[0m"


// 遇到错误直接终止编译
void lexer_error(std::string_view message, int line);
void parser_error(std::string_view message, int line);

#endif