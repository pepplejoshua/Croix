#pragma once

#include <iostream>
#include <string>
#include "Token.h"

using namespace std;

// class to be inherited by abstract base class 
// to allow the template defined types visit this class
template <typename Visitor1>
class Visitable {
public:
    virtual void accept(Visitor1*) = 0;
};

class Binary;
class Unary;
class Grouping;
class Boolean;
class Number;
class String;

// class to be inherited by classes that intend to visit
// exprs
class ExprVisitor {
public:
    virtual void visitBinaryExpr(Binary* expr) = 0;
    virtual void visitUnaryExpr(Unary* expr) = 0;
    virtual void visitGroupingExpr(Grouping* expr) = 0;
    virtual void visitBooleanExpr(Boolean* expr) = 0;
    virtual void visitNumberExpr(Number* expr) = 0;
    virtual void visitStringExpr(String* expr) = 0;
};

// anything that is an ExprVisitor can visit this class
class Expr : public Visitable<ExprVisitor> {
public:
};

class Binary : public Expr {
public:
    Binary(Expr* left, Token op, Expr* right) {
        this->left = left;
        this->op = op;
        this->right = right;
    }

    void accept(ExprVisitor* ev) {
        ev->visitBinaryExpr(this);
    }

Expr* left;
Token op;
Expr* right;
};

class Unary : public Expr {
public:
    Unary(Token op, Expr* right) {
        this->op = op;
        this->right = right;
    }

    void accept(ExprVisitor* ev) {
        ev->visitUnaryExpr(this);
    }
    
    Token op;
    Expr* right;
};

class Grouping : public Expr {
public:
    Grouping(Expr* expr) {
        this->expr = expr;
    }

    void accept(ExprVisitor* ev) {
        ev->visitGroupingExpr(this);
    }
    
    Expr* expr;
};

class Boolean : public Expr {
public:
    Boolean(bool value) {
        this->value = value;
    }
    
    void accept(ExprVisitor* ev) {
        ev->visitBooleanExpr(this);
    }

    bool value;
};

class Number : public Expr {
public:
    Number(double value) {
        this->value = value;
    }

    void accept(ExprVisitor* ev) {
        ev->visitNumberExpr(this);
    }

    double value;
};

class String : public Expr {
public:
    String(string value) {
        this->value = value;
    }
    
    void accept(ExprVisitor* ev) {
        ev->visitStringExpr(this);
    }

    string value;
};


