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
        // methods2 = methods;
        this->methods = methods;
    }

    Storable* call(CInterpreter* in, vector < Storable* > args) {
        return new CroixClassInstance(this);
    }

    int arity() {
        return 0;
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
            // fields2.insert(pair <string, Storable* >(name.lexeme, newVal));
            fields->define(name.lexeme, newVal);
        }

        CroixClass* definition;
        // would an Environment be better for this?
        Environment* fields; 
        // map < string, Storable *> fields2;
    };

    string cName;
    Environment* methods;
    // map < string, Storable *> methods2;
};