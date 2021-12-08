#pragma once

#include <iostream>
#include "Expr.h"
#include <string>
#include <vector>
#include <math.h>

using namespace std;

class Expr;
class Binary;
class Unary;
class Grouping;
class Boolean;
class Number;
class String;
class Nil;

class AstPrinter : public ExprVisitor<string> {
public:
    AstPrinter() {}

    string print(Expr* e) {
        return e->accept(this);
    }  

    string visitBinaryExpr(Binary* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->left);
        exprs.push_back(e->right);
        return parenthesize(e->op.String(), exprs);
    }

    string visitUnaryExpr(Unary* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->right);
        return parenthesize(e->op.String(), exprs);
    }

    string visitGroupingExpr(Grouping* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->expr);
        return parenthesize("grouped", exprs);
    }

    string visitBooleanExpr(Boolean* e) {
        return e->value ? "true" : "false";
    }

    string visitNumberExpr(Number* e) {
        double n = e->value;

        if (floor(n) != n) // we have a decimal part
            return to_string(n);
        else
            return to_string((int) n);
    }

    string visitStringExpr(String* e) {
        return "'" + e->value + "'";
    }

    string visitNilExpr(Nil* e) {
        return "nil";
    }


private:
    string parenthesize(string tag, vector < Expr * > exprs) {
        string o = "";
        o.append("(").append(tag);
        for (int i = 0; i < exprs.size(); ++i) {
            Expr *e = exprs[i];
            o.append(" ");
            o.append(e->accept(this)); // move onto the next expr and represent it
        }

        o.append(")");
        return o;
    }
};