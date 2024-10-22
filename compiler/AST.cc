#include<variant>

#include "error.h"
#include "lexer.h"
#include "AST.h"

void CType::print(int tabs) {
    switch (storage) {
        case CS::STATIC: cout << "static "; break;
        case CS::EXTERN: cout << "extern "; break;
    }
    switch (modifier) {
        case CS::UNSIGNED: cout << "unsigned "; break;
    }
    switch (type) {
        case CS::STRUCT: cout << "struct" << name; break;
        case CS::VOID: cout << "void"; break;
        case CS::CHAR: cout << "char"; break;
        case CS::INT: cout << "int"; break;
        case CS::LONG: cout << "long"; break;
        case CS::DOUBLE: cout << "double"; break;
    }
}

void CDecl::print(int tabs) {
    cout << string(depth, '*') << name;
    if (!parameters.empty()) {
        cout << "(";
        for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            it->print(tabs);
            if (it + 1 < parameters.end()) {
                cout << ", ";
            }
        }
        cout << ")";
    }
    if (!indexes.empty()) {
        for (auto& index : indexes) {
            cout << "[";
            index->print(tabs);
            cout << "]";
        }
    }
}

void Parameter::print(int tabs) {
    type.print();
    if (!decl.name.empty()) {
        cout << " ";
        decl.print();
    }
}


// AST
void Program::print(int tabs) {
    cout << string(2 * tabs, ' ') << "Program(" << endl;
    for (auto& ptr : decls) {
        ptr->print(tabs + 1);
    }
    cout << string(2 * tabs, ' ') << ")" << endl;
}


// expression
void Expression::print(int tabs) {
    cout << string(2 * tabs, ' ') << val << endl;
}


// statement
void Statement::print(int tabs) {
    cout << string(2 * tabs, ' ');
    switch (statement_type) {
        case ST::CONTINUE:
            cout << "continue";
            break;
        case ST::BREAK:
            cout << "break";
            return;
    }
    cout << ";" << endl;
}

void ReturnStatement::print(int tabs) {
    cout << string(2 * tabs, ' ') << "Return(" << endl;
    exp->print(tabs + 1);
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void IfStatement::print(int tabs) {
    cout << string(2 * tabs++, ' ') << "If: " << endl;
    cout << string(2 * tabs, ' ') << "cond = ";
    cond->print(tabs + 1);
    cout << " )" << endl;
    cout << string(2 * tabs, ' ') << "then = " << endl;
    then->print(tabs + 1);
    if (_else) {
        cout << string(2 * tabs, ' ') << "else: " << endl;
        _else->print(tabs + 1);
    }
}

void WhileStatement::print(int tabs) {

}

void DoStatement::print(int tabs) {

}

void ForStatement::print(int tabs) {

}

void Block::print(int tabs) {
    cout << string(2 * tabs, ' ') << "Block(" << endl;
    for (auto& ptr : items) {
        ptr->print(tabs + 1);
    }
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void ExpStatement::print(int tabs) {
    exp->print(tabs);
}

// declaration
void Initializer::print(int tabs) {
    if (init_list.empty()) {
        exp->print(tabs + 1);
    } else {
        cout << "{";
        for (auto it = init_list.begin(); it != init_list.end(); ++it) {
            (*it)->print(tabs + 1);
            if (it + 1 < init_list.end()) {
                cout << ", ";
            }
        }
        cout << "}";
    }
}

void Variable::print(int tabs) {
    cout << string(2 * tabs++, ' ') << "Variable(" << endl
         << string(2 * tabs, ' ') << "type: ";
    type.print();
    cout << endl << string(2 * tabs, ' ') << "declarator: ";
    decl.print(tabs);
    if (initializer) {
        cout << endl << string(2 * tabs, ' ') << "initializer: ";
        initializer->print(tabs);
    }
    cout << endl << string(2 * --tabs, ' ') << endl;
    cout << string(2 * tabs, ' ') << ")" << endl;
}

void Function::print(int tabs) {
    cout << string(2 * tabs++, ' ') << "Function(" << endl
         << string(2 * tabs, ' ') << "signature = ";
    type.print();
    cout << " ";
    decl.print(tabs);
    cout << endl << string(2 * tabs, ' ') << "body = " << endl; 
    body->print(tabs + 1);
    cout << string(2 * --tabs, ' ') << ")" << endl;
}


void Struct::print(int tabs) {
    // cout << string(2 * tabs++, ' ') << "Struct(" << endl
    //      << string(2 * tabs, ' ') << "name: " << type.name;
}
