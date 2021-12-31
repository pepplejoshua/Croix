#pragma once

#include <iostream>
#include <vector>
#include "Expr.h"
#include "../Environment/Environment.h"
#include "../Interpreter/Interpreter.h"
#include "../Helpers/ErrHandler.h"
#include <time.h>

using namespace std;

class CInterpreter : public ExprVisitor<Storable *>, public StmtVisitor<void> {
public:
    virtual void executeBlock(Block* e, Environment* sc) = 0;
    ErrHandler* handler;
    AstPrinter pr;
    bool interacting;
    Environment* env;
};

class Callable : public Storable {
public:
    virtual Storable* call(CInterpreter* in, vector < Storable* > args) = 0;
    virtual int arity() = 0;
    virtual string toString() = 0;
    string storedType() {
        return "Callable";
    }
};


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
            // cout << en << endl;
            // cout << decl->params[i].lexeme << endl;
            // cout << dynamic_cast<Number*>(args[i])->value << endl;
            // cout << endl;
            en->define(decl->params[i].lexeme, args[i]);
        }

        try {
            in->executeBlock(decl->body, en);
        } catch(ReturnExcept* r) {
            return r->value;
        }
        
        return NULL;
    }

    Function* decl;
    Environment* closure;
};