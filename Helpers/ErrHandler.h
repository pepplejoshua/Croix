#pragma once

#include <string>

using namespace std;

struct ErrHandler {
    bool SOURCE_HAD_ERROR = false; // triggered when an error is reported

    // takes a line that caused and error and reports a message
    void error(int line, string msg) {
        report(line, "", msg);
    }

    // reports a msg about where in line causes an error
    void report(int line, string where, string msg) {
        cout << "Err<{" << line << "}> -> " << where << ": " << msg << endl;
        SOURCE_HAD_ERROR = true;
    }
};