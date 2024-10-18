#include "error.h"

std::set<CError> error_record;

void print_error(const std::string_view message, const std::string_view line, int row, int col) {
    cerr << COLOR_ERROR << "error: " << COLOR_RESET << message << endl
         << std::format("{:>5}", std::to_string(row)) << "  | " << line << endl
         << "       | " << string(col, ' ')
         << COLOR_ERROR << '^' << COLOR_RESET << endl;
}

void error_summary(string filename) {
    int n = error_record.size();
    if (n == 0) {
        return;
    }
    int line_number = -1;
    string line;
    std::ifstream file(filename);
        if (!file) {
        cerr << "Failed to open the file." << endl;
        exit(1);
    }
    for (CError e : error_record) {
        while (line_number != e.row) {
            std::getline(file, line);
            ++line_number;
        }
        print_error(e.message, line, e.row, e.col);
    }
    cout << n << " error" << (n > 1 ? "s" : "") << " generated." << endl
         << "Compiler stopped before parsing." << endl;
    file.close();
    exit(1);
}

// void parsing_error(const string& message, int row, int col) {
//     print_error(message, line, row, col);
//     cerr << "Parsing stopped." << endl;
//     file.close();
//     exit(1);
// }