#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Expr.h"
#include "CInterpreter.h"
#include "../Interpreter/Interpreter.h"
#include <time.h>

using namespace std;

class Callable : public Storable {
public:
    virtual Storable* call(CInterpreter* in, vector < Storable* > args) = 0;
    virtual int arity() = 0;
    virtual string toString() = 0;
    string storedType() {
        return "Callable";
    }
};