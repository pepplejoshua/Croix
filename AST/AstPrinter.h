#pragma once

#include <iostream>
#include "Expr.h"
#include <string>
#include <vector>
#include <math.h>

using namespace std;

class AstPrinter : public ExprVisitor {
public:
    AstPrinter() {
        content = "";
    }

    string print(Expr* e) {
        e->accept(this);
        string v = content;
        content = "";
        return v;
    }  

    void visitBinaryExpr(Binary* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->left);
        exprs.push_back(e->right);
        parenthesize(e->op.String(), exprs);
    }

    void visitUnaryExpr(Unary* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->right);
        parenthesize(e->op.String(), exprs);
    }

    void visitGroupingExpr(Grouping* e) {
        vector < Expr * > exprs;
        exprs.push_back(e->expr);
        parenthesize("grouped", exprs);
    }

    void visitBooleanExpr(Boolean* e) {
        content += (e->value ? "true" : "false");
    }

    void visitNumberExpr(Number* e) {
        double n = e->value;

        if (floor(n) != n) // we have a decimal part
            content += to_string(n);
        else
            content += to_string((int) n);
    }

    void visitStringExpr(String* e) {
        content += "'" + e->value + "'";
    }

    void visitNilExpr(Nil* e) {
        content += "nil";
    }


private:
    void parenthesize(string tag, vector < Expr * > exprs) {
        content.append("(").append(tag);
        for (int i = 0; i < exprs.size(); ++i) {
            Expr *e = exprs[i];
            content.append(" ");
            e->accept(this); // move onto the next expr and represent it
        }

        content.append(")");
    }
    string content;
};