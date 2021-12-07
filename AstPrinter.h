#pragma once

#include <iostream>
#include "AST/Expr.h"
#include <string>
#include <vector>

using namespace std;

class AstPrinter : public Visitor<string> {
public:
    string print(Program* expr) {
       return expr->accept(this);
    }

    string visitProgramExpr(Program* p) {
        return p->body->accept(this);
    }

    string visitBinaryExpr(Binary* expr) {
        vector < Expr* > exprs;
        exprs.push_back(expr->left);
        exprs.push_back(expr->right);
        return parenthesize(expr->op.String(), exprs);
    }

    string visitUnaryExpr(Unary* expr) {
        vector < Expr * > exprs;
        exprs.push_back(expr->right);
        return parenthesize(expr->op.String(), exprs);
    }

    string visitGroupingExpr(Grouping* expr) {
        vector < Expr * > exprs;
        exprs.push_back(expr->expr);
        return parenthesize("grouped", exprs);
    }

    string visitBooleanExpr(Boolean* expr) {
        if (expr->value)
            return "true";
        return "false";
    }

    string visitNumberExpr(Number* expr) {
        double n = expr->value;
        return to_string(n);
    }

    string visitStringExpr(String* expr) {
        return expr->value;
    }

    string parenthesize(string name, vector < Expr * > exprs) {
        string o = "";

        o.append("(").append(name);
        for (int i = 0; i < exprs.size(); ++i) {
            Expr *e = exprs[i];
            o.append(" ");
            // o.append(e->accept(this));
        }

        o.append(")");

        return o;
    }
};