#include<variant>

#include "error.h"
#include "lexer.h"
#include "AST.h"

// Notes:
// 1. CType, CDecl, Parameter are printed inline,
//    while others are printed on separate lines.
// 2. Each node executes indent_push() for its child nodes,
//    and excute "cur -= 2" for itself.

vector<int> AST::indent = {};
int AST::cur = 0;

// Print indentation and execute indent.pop_back() if reaching the end.
void AST::print_indent(bool ending) {
    // box-drawing characters: │ ├ └ ─
    int lst = 0;
    cout << string(indent[0] - lst, ' ');
    lst = indent[0] + 1;
    for (size_t i = 1; i != indent.size(); ++i) {
        cout << "│" << string(indent[i] - lst, ' ');
        lst = indent[i] + 1;
    }
    cout << (ending ? "└" : "├");
    for (; lst < cur; ++lst) {
        cout << "─";
    }
    if (ending) {
        indent.pop_back();
        // Pop is executed automatically,
        // but "cur -= 2" needs to be executed afterward.
    }
}

// Record current indentation.
void AST::indent_push() {
    indent.push_back(cur);
    cur += 2;
}

// Print the name of component, then execute indent_push().
// If this function is used, "cur -= 2" needs to be executed afterward.
void AST::print_component(string name, bool ending) {
    print_indent(ending);
    cout << COLOR_COMPONENT << name << COLOR_RESET << endl;
    indent_push();
}

void CType::print() {
    cout << COLOR_TYPE;
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
    cout << COLOR_RESET;
}

// AST
void Program::print(bool ending) {
    indent_push();
    cout << COLOR_CLASS << "Program" << COLOR_RESET << endl;
    for (auto it = decls.begin(); it != decls.end(); ++it) {
        (*it)->print(it + 1 == decls.end());
    }
}


// expression
void Expression::print(bool ending) {
    print_indent(ending);
    cout << val << endl;
    if (ending) {
        cur -= 2;
    }
}


// statement
void Statement::print(bool ending) {
    if (statement_type == ST::NONE) {
        return;
    }
    print_indent(ending);
    switch (statement_type) {
        case ST::CONTINUE:
            cout << COLOR_CLASS << "continue" << COLOR_RESET;
            return;
        case ST::BREAK:
            cout << COLOR_CLASS << "break" << COLOR_RESET;
            return;
    }
}

void ReturnStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Return " << COLOR_RESET << endl;
    indent_push();
    exp->print(true);
    if (ending) {
        cur -= 2;
    }
}

void IfStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "If" << COLOR_RESET << endl;
    indent_push();
    // condition
    print_component("condition", false);
    cond->print(true);
    // then
    print_component("then", !(bool)_else);
    then->print(true);
    // else
    if (_else) {
        print_component("else", true);
        _else->print(true);
    }
    cur -= (ending ? 4 : 2);
}

void WhileStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "While" << COLOR_RESET << endl;
    indent_push();
    // condition
    print_component("condition", false);
    cond->print(true);
    // body
    print_component("body", true);
    body->print(true);
    cur -= (ending ? 4 : 2);
}

void DoStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Do" << COLOR_RESET << endl;
    indent_push();
    // body
    print_component("body", false);
    body->print(true);
    // condition
    print_component("condition", true);
    cond->print(true);
    cur -= (ending ? 4 : 2);
}

void ForStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "For" << COLOR_RESET << endl;
    indent_push();
    // init
    print_component("initialization", false);
    for_init->print(true);
    // condition
    if (cond) {
        print_component("condition", false);
        cond->print(true);
    }
    // increment
    if (inc) {
        print_component("increment", false);
        inc->print(true);
    }
    // body
    print_component("body", true);
    body->print(true);
    cur -= (ending ? 4 : 2);
}

void Block::print(bool ending) {
    // indent_push();
    // print_indent(ending);
    // cout << COLOR_CLASS <<  "Block" << COLOR_RESET << endl;
    // for (auto it = items.begin(); it != items.end(); ++it) {
    //     (*it)->print(it + 1 == items.end());
    // }
    // cur -= 2;
    if (items.empty() && ending) {
        indent.pop_back();
        cur -= 2;
    }
    for (auto it = items.begin(); it != items.end(); ++it) {
        (*it)->print(it + 1 == items.end());
    }
}

void ExpStatement::print(bool ending) {
    exp->print(ending);
}

// declaration
void CDecl::print(bool ending) {
    cout << string(depth, '*') << name;
    if (!parameters.empty()) {
        cout << "(";
        for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            it->print(false);
            if (it + 1 < parameters.end()) {
                cout << ", ";
            }
        }
        cout << ")";
    }
    if (!indexes.empty()) {
        for (auto& index : indexes) {
            cout << "[";
            index->print(false);
            cout << "]";
        }
    }
}

void Parameter::print(bool ending) {
    type.print();
    if (!decl.name.empty()) {
        cout << " ";
        decl.print(false);
    }
}

void Initializer::print(bool ending) {
    if (init_list.empty()) {
        exp->print(ending);
    } else {
        cout << "{";
        for (auto it = init_list.begin(); it != init_list.end(); ++it) {
            (*it)->print(false);
            if (it + 1 < init_list.end()) {
                cout << ", ";
            }
        }
        cout << "}";
        cout << endl;
    }
}

void Variable::print(bool ending) {
    print_indent(ending);    
    cout << COLOR_CLASS << "Varible" << COLOR_RESET << endl;
    indent_push();
    // type
    print_component("type", false);
    print_indent(true);
    type.print();
    cout << endl;
    cur -= 2;
    // declarator
    print_component("declarator", !(bool)initializer);
    print_indent(true);
    decl.print(false);
    cout << endl;
    cur -= 2;
    // initializer
    if (initializer) {
        print_component("initializer", true);
        initializer->print(true);
    }
    cur -= (ending ? 4 : 2);
}

void Function::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Function" << COLOR_RESET << endl;
    indent_push();
    // signature
    print_component("signature", false);
    print_indent(true);
    type.print();
    cout << " ";
    decl.print(false);
    cout << endl;
    cur -= 2;
    // body
    print_component("body", true);
    body->print(true);
    cur -= (ending ? 4 : 2);
}


void Struct::print(bool ending) {
    // cout << string(2 * indent++, ' ') << "Struct(" << endl
    //      << string(2 * indent, ' ') << "name: " << type.name;
}
