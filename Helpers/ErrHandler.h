#pragma once

#include <string>
#include "../AST/TokenTypes.h"

using namespace std;

class RuntimeError : public exception {
public:
    RuntimeError(Token eT, string eMsg) {
        msg = eMsg;
        t = eT;
    }

    virtual const char * what() const throw() {
        return msg.c_str();
    }

    string msg;
    Token t;
 };

class ParseError : public exception { };

class ErrHandler {
public:
    bool SOURCE_HAD_ERROR; // triggered when an error is reported
    bool RUNTIME_ERROR; // triggered for runtime error

    // takes a line tha t caused and error and reports a message
    void error(int line, string msg) {
        report(line, "", msg);
    }

    void error(Token t, string msg) {
        if (t.type == EOF_)
            report(t.line, " at end", msg);
        else
            report(t.line, " at '" + t.lexeme + "'", msg);
    }

    // reports a msg about where in line causes an error
    void report(int line, string where, string msg) {
        cout << "Err<{" << line << "}> -> " << where << ": " << msg << endl;
        SOURCE_HAD_ERROR = true;
    }

    void runtimeError(RuntimeError err) {
        report(err.t.line, "", err.msg);
        SOURCE_HAD_ERROR = false;
        RUNTIME_ERROR = true;
    }
};