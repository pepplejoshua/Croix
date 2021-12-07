#pragma once

#include <string>
#include "../AST/TokenTypes.h"

using namespace std;

struct ErrHandler {
    bool SOURCE_HAD_ERROR = false; // triggered when an error is reported

    // takes a line that caused and error and reports a message
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
};