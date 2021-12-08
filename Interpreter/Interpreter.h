#pragma once

#include <iostream>
#include "../AST/Expr.h"
#include "../AST/TokenTypes.h"
#include "../AST/AstPrinter.h"
#include "../Helpers/ErrHandler.h"

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

class Interpreter : public ExprVisitor<Expr*> {
public:
    Interpreter(ErrHandler* e) { 
        handler = e;
    }

    Expr* eval(Expr* in) {
        return in->accept(this);
    }

    Expr* visitBinaryExpr(Binary* e) {
        Expr *l = eval(e->left);
        Expr *r;
        if (e->op.type != QUESTION_MARK)
            r = eval(e->right);

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
            default: {
                return NULL;
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
            }
        }
    }  

    Expr* visitUnaryExpr(Unary* e) {
        Expr *r = eval(e->right);

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

    Expr* visitGroupingExpr(Grouping* e) {
        return eval(e->expr);
    }

    Expr* visitBooleanExpr(Boolean* e) {
        return e;
    }

    Expr* visitNumberExpr(Number* e) {
        return e;
    }

    Expr* visitStringExpr(String* e) {
        return e;
    }

    Expr* visitNilExpr(Nil* e) {
        return e;
    }

    void interpret(Expr* e) {
        try {
            Expr *r = eval(e);
            if (r && r->type() != '\0') {
                cout << pr.print(r) << endl;
            }

        } catch (RuntimeError& err) {
            handler->runtimeError(err);
        }
        
    }

private:
    ErrHandler* handler;
    AstPrinter pr;
};