//
// Expr.h
// Croix
//
// Auto-generated by Joshua Pepple on 2022-01-08.
// CAUTION: Do not hand edit! Edit gen_ast.py instead.
//

#pragma once

#include <iostream>
#include <string>
#include "Token.h"

using namespace std;

class Assign;
class Binary;
class Unary;
class Grouping;
class Boolean;
class Number;
class String;
class Nil;
class Variable;
class Logical;
class Call;
class Get;
class Set;
class This;
class Super;

// class to be inherited by abstract base class
// to allow the template defined types visit this class
// it is visited by V1 and returns R1
// it is visited by V2 and returns R2
template < typename V1, typename R1, typename V2, typename R2, typename V3, typename R3 >
class Visitable {
public:
    virtual R1 accept(V1) = 0;
    virtual R2 accept(V2) = 0;
    virtual R3 accept(V3) = 0;
};

// class to be inherited by classes that intend to visit
template < typename ReturnValue >
class ExprVisitor {
public:
    virtual ReturnValue visitAssignExpr(Assign*) = 0;
    virtual ReturnValue visitBinaryExpr(Binary*) = 0;
    virtual ReturnValue visitUnaryExpr(Unary*) = 0;
    virtual ReturnValue visitGroupingExpr(Grouping*) = 0;
    virtual ReturnValue visitBooleanExpr(Boolean*) = 0;
    virtual ReturnValue visitNumberExpr(Number*) = 0;
    virtual ReturnValue visitStringExpr(String*) = 0;
    virtual ReturnValue visitNilExpr(Nil*) = 0;
    virtual ReturnValue visitVariableExpr(Variable*) = 0;
    virtual ReturnValue visitLogicalExpr(Logical*) = 0;
    virtual ReturnValue visitCallExpr(Call*) = 0;
    virtual ReturnValue visitGetExpr(Get*) = 0;
    virtual ReturnValue visitSetExpr(Set*) = 0;
    virtual ReturnValue visitThisExpr(This*) = 0;
    virtual ReturnValue visitSuperExpr(Super*) = 0;
};

// used by Environment to store both Exprs
// and Callables like native functions and
// user defined functions

class Storable {
public:
    virtual string storedType() = 0;
};

// anything that is an ExprVisitor can visit this class
class Expr : public Visitable < ExprVisitor < string > *, string, ExprVisitor < Storable * > *, Storable *, ExprVisitor < void > *, void >, public Storable {
public:
    virtual char type() const = 0;

    string storedType() {
        return "Expr";
    }

    virtual ~Expr() { }
};

class Assign : public Expr {
public:
    Assign(Token name, Expr* value) {
        this->name = name;
        this->value = value;
    }
    
    ~Assign() {
        delete this->value;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitAssignExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitAssignExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitAssignExpr(this);
    }
    
    char type() const {
        return '\0';
    }

    Token name;
    Expr* value;
};

class Binary : public Expr {
public:
    Binary(Expr* left, Token op, Expr* right) {
        this->left = left;
        this->op = op;
        this->right = right;
    }
    
    ~Binary() {
        delete this->left;
        delete this->right;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitBinaryExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitBinaryExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitBinaryExpr(this);
    }
    
    char type() const {
        return 'b';
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
    
    ~Unary() {
        delete this->right;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitUnaryExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitUnaryExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitUnaryExpr(this);
    }
    
    char type() const {
        return 'U';
    }

    Token op;
    Expr* right;
};

class Grouping : public Expr {
public:
    Grouping(Expr* expr) {
        this->expr = expr;
    }
    
    ~Grouping() {
        delete this->expr;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitGroupingExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitGroupingExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitGroupingExpr(this);
    }
    
    char type() const {
        return 'G';
    }

    Expr* expr;
};

class Boolean : public Expr {
public:
    Boolean(bool value) {
        this->value = value;
    }
    
    ~Boolean() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitBooleanExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitBooleanExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitBooleanExpr(this);
    }
    
    char type() const {
        return 'B';
    }

    bool value;
};

class Number : public Expr {
public:
    Number(double value) {
        this->value = value;
    }
    
    ~Number() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitNumberExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitNumberExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitNumberExpr(this);
    }
    
    char type() const {
        return 'N';
    }

    double value;
};

class String : public Expr {
public:
    String(string value) {
        this->value = value;
    }
    
    ~String() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitStringExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitStringExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitStringExpr(this);
    }
    
    char type() const {
        return 's';
    }

    string value;
};

class Nil : public Expr {
public:
    Nil() {
    }
    
    ~Nil() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitNilExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitNilExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitNilExpr(this);
    }
    
    char type() const {
        return '\0';
    }
};

class Variable : public Expr {
public:
    Variable(Token name) {
        this->name = name;
    }
    
    ~Variable() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitVariableExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitVariableExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitVariableExpr(this);
    }
    
    char type() const {
        return 'v';
    }

    Token name;
};

class Logical : public Expr {
public:
    Logical(Expr* left, Token op, Expr* right) {
        this->left = left;
        this->op = op;
        this->right = right;
    }
    
    ~Logical() {
        delete this->left;
        delete this->right;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitLogicalExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitLogicalExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitLogicalExpr(this);
    }
    
    char type() const {
        return 'L';
    }

    Expr* left;
    Token op;
    Expr* right;
};

class Call : public Expr {
public:
    Call(Expr* callee, Token rParen, vector < Expr* > arguments) {
        this->callee = callee;
        this->rParen = rParen;
        this->arguments = arguments;
    }
    
    ~Call() {
        delete this->callee;
        for(int i = 0; i < arguments.size(); ++i) {
            delete arguments[i];
        }
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitCallExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitCallExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitCallExpr(this);
    }
    
    char type() const {
        return 'C';
    }

    Expr* callee;
    Token rParen;
    vector < Expr* > arguments;
};

class Get : public Expr {
public:
    Get(Expr* object, Token name) {
        this->object = object;
        this->name = name;
    }
    
    ~Get() {
        delete this->object;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitGetExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitGetExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitGetExpr(this);
    }
    
    char type() const {
        return 'g';
    }

    Expr* object;
    Token name;
};

class Set : public Expr {
public:
    Set(Expr* object, Token name, Expr* value) {
        this->object = object;
        this->name = name;
        this->value = value;
    }
    
    ~Set() {
        delete this->object;
        delete this->value;
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitSetExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitSetExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitSetExpr(this);
    }
    
    char type() const {
        return 'S';
    }

    Expr* object;
    Token name;
    Expr* value;
};

class This : public Expr {
public:
    This(Token keyword) {
        this->keyword = keyword;
    }
    
    ~This() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitThisExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitThisExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitThisExpr(this);
    }
    
    char type() const {
        return 'T';
    }

    Token keyword;
};

class Super : public Expr {
public:
    Super(Token keyword, Token property) {
        this->keyword = keyword;
        this->property = property;
    }
    
    ~Super() {
    }
    
    string accept(ExprVisitor< string >* ev) {
        return ev->visitSuperExpr(this);
    }
    
    Storable * accept(ExprVisitor< Storable * >* ev) {
        return ev->visitSuperExpr(this);
    }
    
    void accept(ExprVisitor< void >* ev) {
        ev->visitSuperExpr(this);
    }
    
    char type() const {
        return 'p';
    }

    Token keyword;
    Token property;
};