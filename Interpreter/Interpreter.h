#pragma once

#include <iostream>
#include <vector>
#include "../AST/Expr.h"
#include "../AST/Stmt.h"
#include "../AST/TokenTypes.h"
#include "../AST/AstPrinter.h"
#include "../AST/Callable.h"
#include "../AST/Class.h"
#include "../AST/Functions.h"
#include "../Helpers/ErrHandler.h"
#include "../Environment/Environment.h"

using namespace std;

bool isTruthy(Expr *e) {
    switch(e->type()) {
        case 'N': { // number
            double n = dynamic_cast<Number *>(e)->value;
            if (n <= 0)
                return false;
            return true;
            break;
        }
        // string
        case 's': {
            string s = dynamic_cast<String *>(e)->value;
            if (s == "")
                return false;
            return true;
            break;
        }
        // NIL
        case '\0': {
            return false;
            break;
        }
        // actual boolean
        case 'B': {
            return dynamic_cast<Boolean *>(e)->value;
            break;
        }
        default:
            return true;
    }
}

bool areEqual(Expr *a, Expr *b) {
    if (a->type() == b->type() == '\0') // both Nil
        return true;
    
    if (a->type() == b->type()) {
        switch(a->type()) {
            case 'N': {
                double rn = dynamic_cast<Number *>(a)->value;
                double ln = dynamic_cast<Number *>(b)->value;
                return rn == ln;
                break;
            }
            case 's': {
                string s1 = dynamic_cast<String *>(a)->value;
                string s2 = dynamic_cast<String *>(b)->value;
                return s1 == s2;
                break;
            }
            case 'B': {
                bool b1 = dynamic_cast<Boolean *>(a)->value;
                bool b2 = dynamic_cast<Boolean *>(b)->value;
                return b1 == b2;
                break;
            }
            default:
                return false;
        }
    } else 
        return false;
}

bool isN(Expr *a) {
    return a->type() == 'N';
}

bool isStr(Expr *a) {
    return a->type() == 's';
}

bool isBool(Expr *a) {
    return a->type() == 'B';
}

bool isNumber(Token op, Expr *e) {
    if (isN(e)) return true;
    throw RuntimeError(op, "Operand must be a Number.");
}

bool areNumbers(Token op, Expr *a, Expr *b) {
    if (isN(a) && isN(b)) return true;
    throw RuntimeError(op, "Operands must be 2 Numbers.");
}

bool areStrings(Token op, Expr *a, Expr *b) {
    if (isStr(a) && isStr(b)) return true;
    throw RuntimeError(op, "Operands must be 2 Strings.");
}

bool areNumbersOrStrings(Token op, Expr *a, Expr *b) {
    if (isStr(a) && isStr(b))
        return true;
        
    if (isN(a) && isN(b))
        return true;   
    throw RuntimeError(op, "Operands must be 2 Numbers or 2 Strings.");
}

class Interpreter : public CInterpreter {
public:
    Interpreter(ErrHandler* e, bool interactiveMode=false, Environment* globals=NULL) { 
        handler = e;
        interacting = interactiveMode;

        if (globals)
            env = globals;
        else
            env = new Environment(e);

        globals->define("clock", (Storable*) new Clock());
        // used to help resolver integration
        this->globals = globals;
    }

    void resolve(Expr* expr, int scopeDepth) {
        locals.insert(pair < Expr*, int >(expr, scopeDepth));
    }

    Storable* eval(Expr* in) {
        return in->accept(this);
    }

    Storable* visitBinaryExpr(Binary* e) {
        Expr *l = dynamic_cast<Expr *>(eval(e->left));
        Expr *r;
        if (e->op.type != QUESTION_MARK)
            r = dynamic_cast<Expr *>(eval(e->right));

        switch(e->op.type) {
            case GREATER: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;
                    
                    return new Boolean(ln > rn);
                } else {
                    return NULL;
                }
                break;
            }
            case GREATER_EQUAL: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;
                    
                    return new Boolean(ln >= rn);
                } else {
                    return NULL;
                }
                break;
            }
            case LESS: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;
                    
                    return new Boolean(ln < rn);
                } else {
                    return NULL;
                }
                break;
            }
            case LESS_EQUAL: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;
                    
                    return new Boolean(ln <= rn);
                } else {
                    return NULL;
                }
                break;
            }
            case NOT_EQUAL: {
                return new Boolean(!areEqual(l, r));
                break;
            }
            case EQUAL_EQUAL: {
                return new Boolean(areEqual(l, r));
                break;
            }
            case PLUS: {
                if (areNumbersOrStrings(e->op, l, r)) {
                    if (isN(l)) {
                        double rn = dynamic_cast<Number *>(r)->value;
                        double ln = dynamic_cast<Number *>(l)->value;

                        return new Number(ln+rn);
                    } else {
                        string rn = dynamic_cast<String *>(r)->value;
                        string ln = dynamic_cast<String *>(l)->value;
                    
                        return new String(ln+rn);
                    }
                } else {
                    return NULL;
                }
                break;
            }
            case MINUS: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;

                    return new Number(ln-rn);
                } else {
                    return NULL;
                }
                break;            }
            case MULT: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;

                    return new Number(rn*ln);
                } else {
                    return NULL;
                }
                break;
            }
            case SLASH: {
                if (areNumbers(e->op, l, r)) {
                    double rn = dynamic_cast<Number *>(r)->value;
                    double ln = dynamic_cast<Number *>(l)->value;

                    if (rn != 0) 
                        return new Number(ln / rn);
                    else
                        throw RuntimeError(e->op, "Division by Zero.");
                } else {
                    return NULL;
                }
                break;
            }
            case COMMA: {
                eval(e->left);
                return eval(e->right);
                break;
            }
            case QUESTION_MARK: {
                bool chooseL = isTruthy(l);

                // the right side of this Binary contains our options
                Binary *b = dynamic_cast<Binary *>(e->right);
                if (chooseL) {
                    return eval(b->left);
                } else {
                    return eval(b->right);
                }
                break;
            }
            default: {
                return NULL;
            }
        }
    }  

    Storable* visitUnaryExpr(Unary* e) {
        Expr *r = dynamic_cast<Expr *>(eval(e->right));

        switch(e->op.type) {
            case NOT: {
                return new Boolean(!isTruthy(r));
                break;
            }
            case MINUS: {
                // we do have a number
                if (isNumber(e->op, r)) {
                    double n = dynamic_cast<Number *>(r)->value;
                    return new Number(-n);
                }
                return NULL;
                break;
            }
            default: {
                return NULL;
            }
        }
    }

    Storable* visitGroupingExpr(Grouping* e) {
        return eval(e->expr);
    }

    Storable* visitBooleanExpr(Boolean* e) {
        return e;
    }

    Storable* visitNumberExpr(Number* e) {
        return e;
    }

    Storable* visitStringExpr(String* e) {
        return e;
    }

    Storable* visitNilExpr(Nil* e) {
        return e;
    }

    Storable* visitVariableExpr(Variable* e) {
        // Storable *v = env->get(e->name);
        // return v;    
        return lookupVariable(e->name, e);
    }

    Storable* lookupVariable(Token name, Expr* e) {
        map < Expr*, int >::iterator iDepth = locals.find(e);
        // not recognized as a local variable, check globally
        if (iDepth == locals.end()) {
            return globals->get(name);
        } else {
            return env->getAtDepth(iDepth->second, name);
        }
    }

    Storable* visitAssignExpr(Assign* e) {
        Storable *v = eval(e->value);
        
        map < Expr*, int >::iterator iDepth = locals.find(e);
        if (iDepth == locals.end()) {
            globals->assign(e->name, v);
        } else {
            env->assignAtDepth(iDepth->second, e->name, v);
        }

        return v;
        // env->assign(e->name, v);
        // return v;
    }

    Storable* visitLogicalExpr(Logical* e) {
        Expr* lhs = dynamic_cast<Expr *>(eval(e->left));

        // perform short circuiting appropriately
        // for OR, if the LHS is true, then return it
        if (e->op.type == OR) {
            if (isTruthy(lhs)) return lhs;
        } else { // for AND, if LHS is false, then return it
            if (!isTruthy(lhs)) return lhs;
        }
        
        return eval(e->right);
    }

    Storable* visitCallExpr(Call* e) {    
        Storable* callee = eval(e->callee);
        vector < Storable* > args;

        for (int i = 0; i < e->arguments.size(); ++i) {
            args.push_back(eval(e->arguments[i]));
        }

        Callable* fn = dynamic_cast< Callable *>(callee);

        if (fn == NULL) // not a callable, since it couldn't cast
            throw RuntimeError(e->rParen, "Can only call functions and classes.");

        if (fn->arity() != args.size()) { // wrong function arity
            string eMsg = "Expected ";
            eMsg += to_string(fn->arity()) + " arguments but got ";
            eMsg += to_string(args.size()) + ".";
            throw RuntimeError(e->rParen, eMsg);
        }

        Storable* res = fn->call(this, args);
        return res;    
    }

    Storable* visitGetExpr(Get* g) {
        Storable* lhs = eval(g->object);

        CroixClass::CroixClassInstance* inst = dynamic_cast<CroixClass::CroixClassInstance*>(lhs);

        if (inst != NULL) {
            return inst->get(g->name);
        }

        throw RuntimeError(g->name, "Only class instances have properties.");
    }

    Storable* visitSetExpr(Set* s) {
        Storable* lhs = eval(s->object);

        CroixClass::CroixClassInstance* inst = dynamic_cast<CroixClass::CroixClassInstance*>(lhs);

        if (inst == NULL) {
            throw RuntimeError(s->name, "Only class instances have properties.");
        }
        Storable* newVal = eval(s->value);
        inst->set(s->name, newVal);        
        return newVal;
    }

    Storable* visitThisExpr(This* t) {
        return lookupVariable(t->keyword, t);
    }

    Storable* visitSuperExpr(Super* s) {
        // ASSUMPTION: that depth will always resolve correctly
        int depth = locals[s];

        CroixClass* superclass = (CroixClass*) env->getAtDepth(depth, s->keyword);
        Storable* child = env->getAtDepth(depth - 1, Token(THIS, "this", s->keyword.line));

        UserFunction* method = (UserFunction*) superclass->methods->get(s->property);

        return method->bind(child);
    }

    void showExpr(Expr* v) {
        if (v) {
            if (interacting)
                cout << "\n  " << pr.print(v) << endl;
            else
                cout << pr.print(v) << endl;
        }
    }

    string getExprString(Expr* e) {
        if (e) {
            return pr.print(e);
        }
        return "";
    }

    void visitExpressionStmt(Expression* e) {
        if (interacting) {
            Expr* v = dynamic_cast<Expr *>(eval(e->expr));
            showExpr(v); 
        } else
            eval(e->expr);
    }

    void visitPrintStmt(Print* e) {
        if (e->expr != NULL) {
            Storable* v = eval(e->expr);

            if (v != NULL) {
                if (v->storedType() == "Expr")
                    showExpr(dynamic_cast<Expr *>(v));   
                else if (v->storedType() == "Callable")
                    cout << dynamic_cast<Callable *>(v)->toString() << endl;
                else {
                    cout << v->storedType() << endl;
                }
            }
        } else {
            cout << endl;
        }
    }

    void visitVarStmt(Var* e) {
        Storable* v = NULL;
        if (e->initValue) {
            v = eval(e->initValue);
        } else {
            v = new Nil();
        }

        env->define(e->name.lexeme, v);
    }

    void visitBlockStmt(Block* e) {
        executeBlock(e, new Environment(handler, env));
    }

    void visitIfStmt(If* e) {
        // check condition to see if it is considered
        // truthy
        if (isTruthy(dynamic_cast<Expr *>(eval(e->cond)))) {
            execute(e->then);
        } else if (e->else_ != NULL){
            execute(e->else_);
        }
    }

    void visitWhileStmt(While* e) {
        while (isTruthy(dynamic_cast<Expr *>(eval(e->cond)))) {
            execute(e->body);
        }
    }

    void visitFunctionStmt(Function* e) {
        UserFunction* f = new UserFunction(e, env);
        env->define(e->fnName.lexeme, (Storable*) f);
    }

    void visitReturnStmt(Return* e) {
        Storable* rVal = NULL;
        if (e->value != NULL) rVal = eval(e->value);

        throw ReturnExcept(rVal);
    }

    void visitClassStmt(Class* c) {
        Storable* super = NULL;
        CroixClass* superclass = NULL;
        if (c->superclass != NULL) {
            super = eval(c->superclass);
            // check to see if super is resolved into a CroixClass
            superclass = dynamic_cast<CroixClass*>(super);

            if (superclass == NULL) {
                throw RuntimeError(c->superclass->name, "Superclass must be a class");
            }
        }

        // allows class to refer to itself
        env->define(c->name.lexeme, new Nil());

        if (c->superclass != NULL) {
            env = new Environment(env->handler, env, true);
            env->define("super", superclass);
        }

        Environment* methods = new Environment(NULL, NULL, true);
        // map < string, Storable *> methods;
        for (int i = 0; c->methods.size() > i; ++i) {
            Function* fn = c->methods[i];
            bool isInit = fn->fnName.lexeme == "init";

            // in the case where we have a superclass, all methods in our class
            // capture the env that has a reference to "super"
            // and then we later pop it off
            UserFunction* method = new UserFunction(fn, env, isInit);
            // methods.insert(pair<string, Storable*>(fn->fnName.lexeme, method));
            methods->define(fn->fnName.lexeme, (Storable*) method);
        }

        CroixClass* uc = new CroixClass(c->name.lexeme, superclass, methods);
        
        // after letting methods bind to env with reference to super,
        // we pop off that env and return to its parent.
        if (superclass != NULL) {
            env = env->parent;
        }
        env->assign(c->name, (Storable*) uc);
    }

    void interpret(vector < Stmt* > stmts) {
        try {
            for (int i = 0; i < stmts.size(); ++i) {
                execute(stmts[i]);
            }
        } catch (RuntimeError& err) {
            handler->runtimeError(err);
        }
    }

    void execute(Stmt *s) {
        s->accept(this);
    }

    void executeBlock(Block* e, Environment* scope) {
        Environment* prev = env;
        try {
            // set new scope and execute statements in this scope
            env = scope;
            for (int i = 0; i < e->stmts.size(); ++i) {
                execute(e->stmts[i]);
            }
        } catch (RuntimeError& err) {
            // even in the case of an error, reset env
            env = prev;
            throw err;
        } catch (ReturnExcept r) { 
            // cause of the dumbest/best 1 off error I have ever experienced in my life
            // I forgot to reset the environment before returning higher up the nested 
            // environment path. I fucking hate C++ pointers. Fuckkkkkkkkkkkkkkkkkkkkk
            env = prev;
            throw r;
        }

        env = prev;
    }
};