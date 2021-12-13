#pragma once

#include <iostream>
#include <vector>
#include "Expr.h"
#include "../Environment/Environment.h"
#include "../Interpreter/Interpreter.h"
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
    virtual Expr* call(CInterpreter* in, vector < Expr* > args) = 0;
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
    Expr* call(CInterpreter* in, vector < Expr* > args) {
        time_t tme;
        time(&tme);
        return new Number((double) tme);
    }
};

class UserFunction : public Callable {
public:
    UserFunction(Function* decl) {
        this->decl = decl;
    }    

    int arity() {
        return decl->params.size();
    }

    string toString() {
        return "<fn " + decl->fnName.lexeme + ">";
    }

    Expr* call(CInterpreter* in, vector < Expr* > args) {
        Environment* en = new Environment(in->handler, in->env);
        for (int i = 0; i < decl->params.size(); ++i) {
            en->define(decl->params[i].lexeme, args[i]);
        }

        in->executeBlock(decl->body, en);
        return NULL;
    }

    Function* decl;
};