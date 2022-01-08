#pragma once

#include "Expr.h"
#include "AstPrinter.h"
#include "../Environment/Environment.h"
#include "../Helpers/ErrHandler.h"
#include "Stmt.h"

class CInterpreter : public ExprVisitor<Storable *>, public StmtVisitor<void> {
public:
    virtual void executeBlock(Block* e, Environment* sc) = 0;
    ErrHandler* handler;
    AstPrinter pr;
    bool interacting;
    Environment* env;
    Environment* globals;
    map < Expr*, int > locals;
};
