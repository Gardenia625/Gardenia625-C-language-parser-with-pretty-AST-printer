#include<variant>

#include "error.h"
#include "lexer.h"
#include "AST.h"

// Notes:
// 1. CType, Declarator, and Parameter are printed inline, while others are printed on separate lines.
// 2. Each node executes "indent_push()" for its child nodes and "cur -= 2" for itself.
//    Thus, every version of the virtual function "print" (except for the one in class Program) 
//    needs to end with "cur -= (ending ? 2 : 0)".
// 3. The function "print_component" will execute "indent_push()" once.
//    Thus, if both "indent_push()" and "print_component(...)" are executed,
//    an additional "cur -= 2" should be executed at the end.
//    In my code, I let the function "print" end with "cur -= (ending ? 4 : 2)" instead in this situation.

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
        // pop is executed automatically, but "cur -= 2" needs to be executed manually.
    }
}

// Record current indentation.
void AST::indent_push() {
    indent.push_back(cur);
    cur += 2;
}

// Print the name of component, then execute indent_push().
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
    if (left) {
        cout << COLOR_OPERATOR << ((bool)mid ? "? :" : name) << COLOR_RESET << endl;
        indent_push();
        left->print(!(bool)right);
        if (mid) {
            mid->print(false);
        }
        if (right) {
            right->print(true);
        }
        
    } else {
        // identifier
        if (call.empty()) {
            cout << name << endl;
        } else {

        }
    }
    cur -= (ending ? 2 : 0);
}

void Constant::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CONST << val << COLOR_RESET << endl;
    cur -= (ending ? 2 : 0);
}

// statement
void Statement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "EmptyStatement" << COLOR_RESET << endl;
    cur -= (ending ? 2 : 0);
}

void ContinueStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Continue" << COLOR_RESET << endl;
    cur -= (ending ? 2 : 0);
}

void BreakStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Break" << COLOR_RESET << endl;
    cur -= (ending ? 2 : 0);
}

void ReturnStatement::print(bool ending) {
    print_indent(ending);
    cout << COLOR_CLASS << "Return " << COLOR_RESET << endl;
    indent_push();
    exp->print(true);
    cur -= (ending ? 2 : 0);
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
    init->print(true);
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
    print_indent(ending);
    cout << COLOR_CLASS <<  "Block" << COLOR_RESET << endl;
    indent_push();
    for (auto it = items.begin(); it != items.end(); ++it) {
        (*it)->print(it + 1 == items.end());
    }
    cur -= (ending ? 2 : 0);
}

void ExpStatement::print(bool ending) {
    exp->print(ending);
}

// declaration
void Declarator::print(bool ending) {
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
        print_component("initializer_list", true);
        for (auto it = init_list.begin(); it != init_list.end(); ++it) {
            (*it)->print(it + 1 == init_list.end());
        }
        cur -= 2;
    }
}

void Variable::print(bool ending) {
    print_indent(ending);    
    cout << COLOR_CLASS << "Varible" << COLOR_RESET << endl;
    indent_push();
    // type
    print_indent(false);
    cout << COLOR_COMPONENT << "type: " << COLOR_RESET;
    type.print();
    cout << endl;
    // declarator
    print_indent(decl.indexes.empty() && !(bool)initializer);
    cout << COLOR_COMPONENT << "declarator: " << COLOR_RESET;
    decl.print(false);
    cout << endl;
    // array size
    if (!decl.indexes.empty()) {
        print_component("array_size", !(bool)initializer);
        for (auto it = decl.indexes.begin(); it != decl.indexes.end(); ++it) {
            (*it)->print(it + 1 == decl.indexes.end());
        }
    }
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
    print_indent(!(bool)body);
    cout << COLOR_COMPONENT << "signature: " << COLOR_RESET;
    type.print();
    cout << " ";
    decl.print(false);
    cout << endl;
    // body
    print_component("body", true);
    body->print(true);
    cur -= (ending ? 4 : 2);
}


void Struct::print(bool ending) {
    
}
