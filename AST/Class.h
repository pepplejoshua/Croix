#pragma once
#include <iostream>
#include <vector>
#include "Callable.h"
// #include "Instance.h"

class LoxClassInstance;

class LoxClass : public Callable {
public:
    LoxClass(string name) {
        cName = name;
    }

    Storable* call(CInterpreter* in, vector < Storable* > args) {
        return new LoxClassInstance(this);
    }

    int arity() {
        return 0;
    }   

    string toString() {
        return cName;
    }
    
    // I despise C++ for this
    class LoxClassInstance : public Storable {
    public:
        LoxClassInstance(LoxClass* loxclass) {
            definition = loxclass;
        }

        string storedType() {
            return "<" + definition->cName + " instance>";
        }

        LoxClass* definition;
    };

    string cName;
};