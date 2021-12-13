#pragma once

#include <iostream>
#include <map>
#include "../AST/Expr.h"
#include "../Helpers/ErrHandler.h"

using namespace std;

class Environment {
public:
    Environment(ErrHandler* h, Environment* enc=NULL) {
        handler = h;
        enclosing = enc;
    }

    // defines a variable local to the present environment
    void define(string varName, Storable* value) {
        map < string, Storable* >::iterator existing = values.find(varName);

        if (existing != values.end()) {
            values.erase(varName);
        }
        values.insert(pair< string, Storable* >(varName, value));
    }

    Storable* localGet(Token name) {
        map < string, Storable* >::iterator existing = values.find(name.lexeme);

        if (existing != values.end()) return existing->second; // if we find it in this env, return its value

        return NULL;
    }

    Storable* get(Token name) {
        Storable* v = localGet(name); // if we can find it locally
        if (v) return v;

        // potential iterative solution, might be faster than recursive solution
        // Environment* e = enclosing;
        // for (e; e != NULL; e = enclosing->enclosing) {
        //     Expr* res = e->get(name);
        //     if (res)
        //         return res;
        // }

        if (enclosing) // if we didn'd find it locally, look further up the chain
            return enclosing->get(name);

        throw RuntimeError(name, "Undefined variable reference '" + name.lexeme + "'.");
    }

    void assign(Token varName, Storable* value) {
        if (localGet(varName)) { // if this is an existing local variable, call define to locally reset it
            define(varName.lexeme, value);
            return;
        } else if (enclosing) { // else check in outer scopes, if possible, and assign to it
            enclosing->assign(varName, value); // try a possible assign in a higher scope
            return;
        }

        // eventually if this is a variable not defined in any of the enclosing scopes, we will
        // get back to Global scope, and throw this RuntimeError
        throw RuntimeError(varName, "Undefined variable reference '" + varName.lexeme + "'.");
    }

private:
    ErrHandler* handler;
    // map < string, Expr* > values;
    map < string, Storable* > values;
    Environment* enclosing;
};