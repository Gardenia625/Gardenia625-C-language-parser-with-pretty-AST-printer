#include "error.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

std::set<CError> error_record;

void print_error(const std::string_view message, const std::string_view line, int row, int col) {
    cerr << COLOR_ERROR << "error: " << COLOR_RESET << message << endl
         << std::format("{:>5}", std::to_string(row)) << "  | " << line << endl
         << "       | " << std::string(col, ' ')
         << COLOR_ERROR << '^' << COLOR_RESET << endl;
}

void error_summary(std::string filename) {
    int n = error_record.size();
    if (n == 0) {
        return;
    }
    int line_number = -1;
    std::string line;
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
    cout << n << " error" << (n > 1 ? "s" : "") << " generated." << endl;
}