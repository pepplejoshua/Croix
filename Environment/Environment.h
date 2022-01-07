#pragma once

#include <iostream>
#include <map>
#include "../AST/Expr.h"
#include "../Helpers/ErrHandler.h"

using namespace std;

class Environment {
public:
    Environment(ErrHandler* h, Environment* par=NULL) {
        handler = h;
        parent = par;
    }

    // defining an identifier in the current scope
    void define(string name, Storable* val) {
        if (find(name) != NULL) // allow redefinition and shadowing
            stored.erase(name);

        stored.insert(pair<string, Storable* >(name, val));
    }

    // for identifier reference
    Storable* get(Token key) {
        Storable* found = find(key.lexeme);

        if (found != NULL)
            return found;

        if (parent != NULL)
            return parent->get(key); // check nested Environments

        throw RuntimeError(key, "Undefined variable reference '" + key.lexeme + "'.");
    }

    Storable* getAtDepth(int distance, Token name) {
        return visitAncestor(distance)->get(name);
    }

    Environment* visitAncestor(int distance) {
        Environment* env = this;
        // walk by specified distance to find 
        // env at right depth
        for (int i = 0; distance > i; ++i) {
            env = env->parent;
        }
        return env;
    }

    // for changing the value of a name, as long as it exists
    void assign(Token key, Storable* val) {
        Storable* found = find(key.lexeme);

        if (found != NULL) {
            stored.erase(key.lexeme);
            stored.insert(pair<string, Storable*>(key.lexeme, val));
            return;
        } 

        if (parent != NULL) {
            parent->assign(key, val); // check nested Environments
            return;
        }
        
        throw RuntimeError(key, "Undefined variable reference '" + key.lexeme + "'.");
    }

    void assignAtDepth(int distance, Token key, Storable* value) {
        visitAncestor(distance)->assign(key, value);
    }

    Storable* find(string name) {
        map < string, Storable* >::iterator loc = stored.find(name);

        if (loc != stored.end()) 
            return loc->second;

        return NULL;            
    }

private:
    ErrHandler* handler;
    map < string, Storable* > stored;
    Environment* parent;
};