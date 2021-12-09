#pragma once

#include <iostream>
#include <map>
#include "../AST/Expr.h"
#include "../Helpers/ErrHandler.h"

using namespace std;

class Environment {
public:
    Environment(ErrHandler* h) {
        handler = h;
    }

    void define(string varName, Expr* value) {
        map < string, Expr* >::iterator existing = values.find(varName);

        if (existing != values.end()) {
            values.erase(varName);
        }
        values.insert(pair< string, Expr* >(varName, value));
    }

    Expr* get(Token name) {
        map < string, Expr* >::iterator existing = values.find(name.lexeme);

        if (existing != values.end()) return existing->second;
        throw RuntimeError(name, "Undefined variable reference '" + name.lexeme + "'.");
    }

private:
    ErrHandler* handler;
    map < string, Expr* > values;
};