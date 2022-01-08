#pragma once

#include "Callable.h"
#include "../Environment/Environment.h"

using namespace std;

class NativeFn : public Callable {
public:
    int arity() {
        return 0;
    }

    string toString() {
        return "<native fn>";
    }
};

class Clock : public NativeFn {
    Storable* call(CInterpreter* in, vector < Storable* > args) {
        time_t tme;
        time(&tme);
        return new Number((double) tme);
    }
};

class UserFunction : public Callable {
public:
    UserFunction(Function* decl, Environment* clos) {
        this->decl = decl;
        this->closure = clos;
    }    

    int arity() {
        return decl->params.size();
    }

    string toString() {
        return "<fn " + decl->fnName.lexeme + ">";
    }

    Storable* call(CInterpreter* in, vector < Storable* > args) {
        Environment* en = new Environment(in->handler, closure);
        for (int i = 0; i < decl->params.size(); ++i) {
            // cout << decl->params[i].lexeme << endl;
            // cout << dynamic_cast<Number*>(args[i])->value << endl;
            // cout << endl;
            en->define(decl->params[i].lexeme, args[i]);
        }

        try {
            in->executeBlock(decl->body, new Environment(closure->handler, en));
        } catch(ReturnExcept r) {
            return r.value;
        }
        
        return NULL;
    }

    UserFunction* bind(Storable* instance) {
        Environment* env = new Environment(NULL, closure, true);
        env->define("this", instance);
        return new UserFunction(decl, env);
    }

    Function* decl;
    Environment* closure;
};