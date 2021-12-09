//
//  croix.cpp
//  Croix
//
//  Created by Joshua Pepple on 2021-12-04.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Lexer/Lexer.h"
#include "Helpers/ErrHandler.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/TokenTypes.h"
#include "AST/Token.h"
#include "AST/AstPrinter.h"
#include "Parser/Parser.h"
#include "Interpreter/Interpreter.h"

using namespace std;

// checks argc for expected count
// shows error message otherwise
bool hasCorrectArgCount(int c);

// reads all the lines at found at path, if it exists
// else it shows an error
string readAllLines(string path);

// takes a file path, reads it's contents and runs it
void runFile(string path);

// takes source code as a string and runs it
void run(string src, bool interact=false);

// runs the repl loop for croix
void runPrompt();

ErrHandler CroixErrManager;

int main(int argc, const char * argv[]) {
    // 34 + (- (2 ^ 3))
    if (hasCorrectArgCount(argc)) {
        if (argc == 2) // user provided a script
            runFile(argv[1]);
        else if (argc == 1) // no path provided
            runPrompt();
    } else {
        return 64; // exit with an usage error
    }
    return 0;
}

// checks argc for expected count
// shows error message otherwise
bool hasCorrectArgCount(int c) {
    if (c > 2) {
        cout << "usage -> crx <{script}>" << endl;
        return false;
    }
    return true;
}

// reads all the lines at found at path, if it exists
// else it shows an error
string readAllLines(string path) {
    ifstream script(path);
    
    string contents;
    if (script.is_open()) { // no problems getting to file and opening it
        string line;
        
        while(getline(script, line)) { // loop till EOF is reached
            contents += line + "\n"; // add each line to contents and add the discarded delimiter back
        }
    } else {
        exit(74); // error while performing IO on some file
    }
    return contents;
}

// takes a file path, reads it's contents and runs it
void runFile(string path) {
    cout << "Running file -> " << path << endl;
    string lines = readAllLines(path);

    cout << "<----------------- File contents ----------------->\n";
    cout << lines << endl;
    cout << "---------------------------------------------------\n\n";
     
    run(lines);
    if (CroixErrManager.SOURCE_HAD_ERROR) exit(65); // incorrect input error
    if (CroixErrManager.RUNTIME_ERROR) exit(70);
}

// takes source code as a string and runs it
void run(string src, bool interact) {
    // do nothing for now
    Lexer crxLex(src, &CroixErrManager);
    vector < Token > tokens = crxLex.lexTokens(); 

    bool v = CroixErrManager.SOURCE_HAD_ERROR;
    
    if (v) 
        return;

    Parser p(tokens, &CroixErrManager);
    vector < Stmt* > stmts = p.parse();

    v = CroixErrManager.SOURCE_HAD_ERROR;

    if (v) 
        return;

    Interpreter in(&CroixErrManager, interact);
    in.interpret(stmts);
}

// runs the repl for interactive program
void runPrompt() {
    cout << "Running prompt" << endl;
    
    string TERMINATE = ".q";
    while (true) { // running loop
        cout << "\n{crx}:: ";
        string line;
        getline(cin, line);
        if (line == "")
            continue; // empty code line, skip
        if (line == TERMINATE) { // terminate repl
            cout << "...bye..." << endl;
            break;
        }
        run(line, true); // execute line
        CroixErrManager.SOURCE_HAD_ERROR = false; // reset flag so it doesn't kill session for user
    }
}
