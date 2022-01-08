#pragma once

#include <iostream>
#include <map>
#include <vector>
#include "../AST/Expr.h"
#include "../AST/Token.h"
#include "../AST/Stmt.h"
#include "../Helpers/ErrHandler.h"
#include "../Interpreter/Interpreter.h"

enum FunctionType { NONE, FUNCTION, METHOD, INITIALIZER };
enum ClassType { NOCLASS, SOMECLASS };

class Resolver : public ExprVisitor<void>, public StmtVisitor<void> {
public:
    Resolver(Interpreter* i, ErrHandler* handler) {
        interpreter = i;
        eHandler = handler;
        currentFunctionType = NONE;
        currentClassType = NOCLASS;
    }

    void visitVarStmt(Var* e) {
        declare(e->name);
        if (e->initValue != NULL) {
            resolve(e->initValue);
        }
        define(e->name);
    }

    void visitClassStmt(Class* c) {
        ClassType enclosing = currentClassType;
        currentClassType = SOMECLASS;
        declare(c->name);
        define(c->name);

        // scope used to capture "this" variable
        enterScope();
        scopes.back().insert(pair<string, bool>("this", true));
        // now handle resolving methods 
        for (int i = 0; c->methods.size() > i; ++i) {
            FunctionType declaration = METHOD;
            if (c->methods[i]->fnName.lexeme == "init") {
                declaration = INITIALIZER;
            }
            resolveFunction(c->methods[i], declaration);
        }
        exitScope();
        currentClassType = enclosing;
    }

    void visitVariableExpr(Variable* e) {
        // there is some local scope, and the top scope contains the referenced name
        if(!scopeIsEmpty() && containsKey(scopes.back(), e->name.lexeme)) {
            map < string, bool > scope = scopes.back();
            // we have just referenced a declared but undefined name
            // or a variable that is shadowing a variable in an outer scope
            if (scope.at(e->name.lexeme) == false) { 
                eHandler->error(e->name, "Can't reference local variable in its own initializer.");
            }
        }
        resolveLocally(e, e->name);
    }

    void visitBlockStmt(Block* e) {
        enterScope();
        resolveStmts(e->stmts);
        exitScope();
    }

    void visitAssignExpr(Assign* e) {
        // handle RHS first
        resolve(e->value);
        // then handle variable name
        resolveLocally(e, e->name);
    }

    void visitGetExpr(Get* g) {
        // we only resolve the lhs of the dot
        // since methods are dynamically handled at runtime
        // we leave the name resolved, till runtime
        resolve(g->object);
    }

    void visitSetExpr(Set* s) {
        resolve(s->object);
        resolve(s->value);
    }

    void visitFunctionStmt(Function* f) {
        declare(f->fnName);
        define(f->fnName);
        resolveFunction(f, FUNCTION);
    }

    void visitExpressionStmt(Expression* e) {
        resolve(e->expr);
    }

    void visitIfStmt(If* e) {
        resolve(e->cond);
        resolve(e->then);

        if (e->else_ != NULL)
            resolve(e->else_);
    }

    void visitPrintStmt(Print* e) {
        if (e->expr != NULL)
            resolve(e->expr);
    }
    
    void visitReturnStmt(Return* e) {
        if (currentFunctionType == NONE) {
            eHandler->error(e->ret, "Can't return from top-level code.");
        }
        

        if (e->value != NULL) {
            if (currentFunctionType == INITIALIZER) {
                eHandler->error(e->ret, "Can't return a value from inside an initializer.");
            }
            resolve(e->value);
        }
    }

    void visitWhileStmt(While* w) {
        resolve(w->cond);
        resolve(w->body);
    }

    void visitBinaryExpr(Binary* b) {
        resolve(b->left);
        resolve(b->right);
    }

    void visitCallExpr(Call* c) {
        resolve(c->callee);
        for (int i = 0; c->arguments.size() > i; ++i) {
            resolve(c->arguments[i]);
        }
    }

    void visitGroupingExpr(Grouping* g) {
        resolve(g->expr);
    }

    void visitBooleanExpr(Boolean* b) {
        return;
    }

    void visitNumberExpr(Number* n) {
        return;
    }

    void visitStringExpr(String* s) {
        return;
    }

    void visitLogicalExpr(Logical* l) {
        resolve(l->left);
        resolve(l->right);
    }

    void visitUnaryExpr(Unary *u) {
        resolve(u->right);
    }

    void visitNilExpr(Nil* e) { }

    void visitThisExpr(This* t) {
        if (currentClassType == NOCLASS) {
            eHandler->error(t->keyword, "Can't use 'this' outside of a class.");
            return;
        }
        resolveLocally(t, t->keyword);
    }

    void resolveStmts(vector < Stmt* > stmts) {
        for (int i = 0; stmts.size() > i; ++i) {
            resolve(stmts[i]);
        }
    }

private:
    void declare(Token name) {
        if (scopeIsEmpty()) // global variable
            return;
        map < string, bool > &curScope = scopes.back();
        // redeclaring a variable or name is an error        
        if (containsKey(curScope, name.lexeme)) {
            eHandler->error(name, "Variable with same name already exists in this scope.");
        }
        // initialization is incomplete, awaiting resolve,
        // so it's set to false
        curScope.insert(pair< string, bool >(name.lexeme, false));
    }

    void define(Token name) {
        if (scopeIsEmpty()) // global variable
            return;
        map < string, bool > &curScope = scopes.back();
        curScope.at(name.lexeme) = true; // successfully resolved
        // cout << name.lexeme << " defined\n";
    }

    void resolve(Stmt* stmt) {
        stmt->accept(this);
    }

    void resolve(Expr* e) {
        e->accept(this);
    }

    void resolveLocally(Expr* e, Token name) {
        for (int i = scopes.size() -1; i >= 0; --i) {
            map < string, bool > scope = scopes[i];
            if (containsKey(scope, name.lexeme)) {
                // cout << "Resolving " << interpreter->getExprString(e) 
                //     << " at depth " << scopes.size() - i - 1 << endl;
                // cout << "total len is " << scopes.size() << endl << endl;
                interpreter->resolve(e, scopes.size() - i - 1);
                return;
            }
        }
    }

    void resolveFunction(Function* f, FunctionType funcType) {
        FunctionType enclosingFunctionType = currentFunctionType;
        currentFunctionType = funcType;

        enterScope();
        for (int i = 0; f->params.size() > i; ++i) {
            Token param = f->params[i];
            declare(param);
            define(param);
        }
        resolve(f->body);
        exitScope();
        currentFunctionType = enclosingFunctionType;
    }

    // simulate the linked list created during runtime inside
    // interpreter, by stacking environments 
    // (but not chained in a linked list)
    void enterScope() {
        map < string, bool > newScope;
        scopes.push_back(newScope);
    }

    // pops top environment
    void exitScope() {
        scopes.pop_back();
    }

    bool scopeIsEmpty() {
        return scopes.size() == 0;
    }

    bool containsKey(map < string, bool > scope, string name) {
        map < string, bool >::iterator elem = scope.find(name);

        if (elem != scope.end())
            return true;
        return false;
    }

    // used to check what type of function we are currently in
    FunctionType currentFunctionType; 
    ClassType currentClassType;

    Interpreter* interpreter;
    ErrHandler* eHandler;
    
    // a stack of Environment scopes
    // where an Environment is map < string, bool >
    vector < map < string, bool> > scopes;
};