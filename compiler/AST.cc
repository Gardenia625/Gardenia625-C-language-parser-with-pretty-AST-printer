#include<variant>

#include "error.h"
#include "lexer.h"
#include "AST.h"

void Program::print(int tabs) {
    cout << string(2 * tabs, ' ') << "Program(" << endl;
    for (auto& ptr : decls) {
        ptr->print(tabs + 1);
    }
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void Function::print(int tabs) {
    cout << string(2 * tabs++, ' ') << "Function(" << endl
         << string(2 * tabs, ' ') << "name = \"" << name << "\"" << endl
         << string(2 * tabs, ' ') << "para = ( ";
    
    for (auto i = 0; i < parameters.size(); ++i) {
        parameters[i]->print(0);
        if (i < parameters.size() - 1) {
            cout << ", ";
        }
    }
    cout << " )" << endl
         << string(2 * tabs, ' ') << "body = (" << endl; 
    body->print(tabs);
    cout << string(2 * tabs--, ' ') << ")" << endl;
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void Block::print(int tabs) {
    for (auto& ptr : items) {
        ptr->print(tabs + 1);
    }
}

void ReturnStatement::print(int tabs) {
    cout << string(2 * tabs, ' ') << "Return(" << endl;
    exp->print(tabs + 1);
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void Variable::print(int tabs) {
    
    cout << string(2 * tabs, ' ') << "int " << name;
}

void Parameter::print(int tabs) {
    if (type == CType::VOID) {
        cout << "void";
    } else {
        cout << "int " << name;
    }
}

void Expression::print(int tabs) {
    cout << string(2 * tabs, ' ') << val << endl;
}