#pragma once

#include <iostream>
#include <map>
#include "Callable.h"
#include "Functions.h"
#include "../Environment/Environment.h"

using namespace std;

class CroixClassInstance;

class CroixClass : public Callable {
public:
    CroixClass(string name, Environment* methods) {
        cName = name;
        this->methods = methods;
    }

    Storable* call(CInterpreter* in, vector < Storable* > args) {
        CroixClassInstance* instance = new CroixClassInstance(this);
        UserFunction* init = dynamic_cast<UserFunction*>(methods->find("init"));
        // some init function was provided
        // so we bind it to instance to allow access to "this"
        // then we call it to init fields as required
        if (init != NULL) {
            init->bind(instance)->call(in, args);
        }
        return instance;
    }

    int arity() {
        UserFunction* init = dynamic_cast<UserFunction*>(methods->find("init"));
        if (init == NULL) {
            return 0;
        }
        return init->arity();
    }   

    string toString() {
        return cName;
    }
    
    // I despise C++ for this
    class CroixClassInstance : public Storable {
    public:
        CroixClassInstance(CroixClass* loxclass) {
            definition = loxclass;
            // allows fields to shadow method definitions
            fields = new Environment(NULL, definition->methods, true);
        }

        string storedType() {
            return "<" + definition->cName + " instance>";
        }

        Storable* get(Token name) {
            Storable* match = fields->get(name);
            
            // we have found a method
            if (match->storedType() == "Callable") {
                UserFunction* method = dynamic_cast<UserFunction*>(match);
                if (method != NULL) {
                   return (Storable*) method->bind(this);
                }
            }
            // Environment* methods = definition->methods;
            // map < string, Storable * >::iterator found = fields2.find(name.lexeme);

            // if (found != fields2.end())
            //     return found->second;
            
            // found = definition->methods2.find(name.lexeme);
            
            // if (found != definition->methods2.end())
            //     return found->second;

            // throw RuntimeError(name, "Undefined property " + name.lexeme + ".");
            return match;
        }

        void set(Token name, Storable* newVal) {
            fields->define(name.lexeme, newVal);
        }

        CroixClass* definition;
        Environment* fields; 
    };

    string cName;
    Environment* methods;
};