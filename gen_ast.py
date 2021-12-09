import sys
from datetime import datetime
from typing import List

class CodeAssembler:
    def __init__(ca) -> None:
        ca.defTabSize = 4
        ca.body = [] 
        ca.tabSize = 0

    def insert(ca, line: str = ''):
        line = (ca.tabSize * " ") + line + "\n"
        ca.body.append(line);

    def unaddLastLine(ca):
        ca.body = ca.body[0:len(ca.body)-1]

    def dedent(ca):
        if (ca.tabSize > 0):
            ca.tabSize -= ca.defTabSize

    def indent(ca):
        ca.tabSize += ca.defTabSize

    def indentInsertDedent(ca, line: str):
        ca.indent()
        ca.insert(line)
        ca.dedent()

    def __str__(ca) -> str:
        return ''.join(ca.body)

# add top comments and include statements
# aka boilerplate
def addTopOfFile(Cpp: CodeAssembler, baseClass: str, importExpr=False):
    Cpp.insert("//") 
    Cpp.insert(f"// {baseClass}.h")
    Cpp.insert("// Croix")
    Cpp.insert("//")
    day = datetime.today().strftime('%Y-%m-%d')
    Cpp.insert(f"// Generated by Joshua Pepple on {day}.")
    Cpp.insert("//")
    Cpp.insert()
    Cpp.insert("#pragma once")
    Cpp.insert()

    Cpp.insert("#include <iostream>")
    Cpp.insert("#include <string>")
    Cpp.insert('#include "Token.h"')
    if importExpr:
        Cpp.insert('#include "Expr.h"')
    Cpp.insert()
    Cpp.insert("using namespace std;")

def defineVisitableGeneric(Cpp: CodeAssembler, pairCount: int, vStr: str="Visitable"):
    Cpp.insert()
    Cpp.insert("// class to be inherited by abstract base class")
    Cpp.insert("// to allow the template defined types visit this class")
    Cpp.insert("// it is visited by V1 and returns R1")
    Cpp.insert("// it is visited by V2 and returns R2")
    
    # use pair count to generate the appropriate pair of Visitor
    # and VisitorReturnVal
    # so each Vistor is next to its Return value in the template
    tStr = "template < "
    acceptFns = []
    for i in range(1, pairCount+1):
        # we are now past the first one, so we can add newline
        # to tStr
        vTag = "V" + str(i)
        rTag = "R" + str(i)
        v = "typename " + vTag
        r = "typename " + rTag
        tStr += v + ', ' + r
        fn = f"virtual {rTag} accept({vTag}) = 0;"
        acceptFns.append(fn)

        if i+1 != pairCount + 1:
            tStr += ', '

    # replace last comma with > to close template 
    tStr = tStr[0:len(tStr)] + " >"
    Cpp.insert(tStr)
    Cpp.insert(f"class {vStr} " + "{")
    Cpp.insert("public:")
    
    for fn in acceptFns:
        Cpp.indentInsertDedent(fn)
    Cpp.insert("};")

def forwardDeclareClasses(Cpp: CodeAssembler, classes: list[str]):
    Cpp.insert()
    
    for cl in classes:
        Cpp.insert(f"class {cl};")

def defineVisitorGeneric(Cpp: CodeAssembler, classes: list[str], baseClass: str, typeTag: str = "Expr"):
    Cpp.insert()
    Cpp.insert("// class to be inherited by classes that intend to visit")
    rTag = "ReturnValue"
    Cpp.insert(f"template < typename {rTag} >")
    Cpp.insert(f"class {typeTag}Visitor " + "{")
    Cpp.insert("public:")
    
    for cl in classes:
        ln = f"virtual {rTag} visit{cl}{baseClass}({cl}*) = 0;"
        Cpp.indentInsertDedent(ln)
    Cpp.insert("};")

def defineBaseClass(Cpp: CodeAssembler, baseClass: str, stmt=False):
    Cpp.insert()
    Cpp.insert("// anything that is an ExprVisitor can visit this class")


    # build the Visitable inheritance statement:
    returns : list[str]

    if stmt:
        returns = ["void"]
    else:
        returns = ["string", f"{baseClass} *"]

    if stmt:
        inher = "public VisitableStmt < "
        #  build the Visitable inheritance statement:
    else:
        inher = "public Visitable < "

    for i in range(len(returns)):
        r = returns[i]
        if stmt:
            inher += f"StmtVisitor < {r} > *, {r}"
        else:
            inher += f"ExprVisitor < {r} > *, {r}"

        if i+1 != len(returns):
            inher += ', '
    inher += " >"

    Cpp.insert(f"class {baseClass} : {inher} " + "{")
    Cpp.insert("public:")
    Cpp.indentInsertDedent("virtual char type() const = 0;")
    Cpp.insert("};")

def defineType(Cpp: CodeAssembler, baseClass: str, className: str, fieldList: str, stmt: bool = False):
    # start of new class 
    Cpp.insert(f"class {className} : public {baseClass} " + "{")

    Cpp.insert("public:")
    # indent into definition of class
    Cpp.indent()
    # constructor
    Cpp.insert(f"{className}({fieldList}) " + "{")

    # constructor body
    fields = fieldList.split(', ')
    for f in fields:
        if f != '':
            varName = f.split(" ")[1]
            Cpp.indentInsertDedent(f"this->{varName} = {varName};")

    Cpp.insert("}")

    # generate necessary accept functions
    returns : list[str]

    if stmt:
        returns = ["void"]
    else:
        returns = ["string", f"{baseClass}*"]
    
    for r in returns:
        Cpp.insert()
        if stmt:
            fnDef = f"{r} accept(StmtVisitor< {r} >* ev) " + "{"
        else:
            fnDef = f"{r} accept(ExprVisitor< {r} >* ev) " + "{"
        Cpp.insert(fnDef)

        if r == "void":
            Cpp.indentInsertDedent(f"ev->visit{className}{baseClass}(this);")
        else:
            Cpp.indentInsertDedent(f"return ev->visit{className}{baseClass}(this);")
        Cpp.insert("}")
    
    Cpp.insert()
    Cpp.insert("char type() const {")
    mp = {
        "Binary" : 'b',
        "Unary": 'U',
        "Grouping": 'G',
        "Boolean": 'B',
        "Number": 'N',
        "String": 's',
        "Expression" : 'E',
        "Print": 'P',
        "Var": 'V',
        "Variable": 'v',
    }
    tag = mp.get(className, None)
    if tag:
        Cpp.indentInsertDedent(f"return '{tag}';")
    else:
        Cpp.indentInsertDedent("return \'\\0\';")
    Cpp.insert("}")
    Cpp.dedent()

    Cpp.insert()
    UNNEEDEDSPACE = False
    for f in fields:
        if f != '':
            Cpp.indentInsertDedent(f+';')
        else:
            UNNEEDEDSPACE = True
            break
    if UNNEEDEDSPACE: # used Nil to keep formatting nice
        Cpp.unaddLastLine()

    Cpp.insert("};")
    Cpp.dedent()

def writeOut(path: str, Cpp: CodeAssembler):
    print("writing to", path)
    with open(path, 'w+') as astFile:
        astFile.write(str(Cpp))

def generateExprHeaderForTypes(outDir: str, baseClass: str, types: list[str]):
    outPath = "./" + outDir + '/' + baseClass +'.h'

    Cpp = CodeAssembler()
    addTopOfFile(Cpp, baseClass)    

    classes = [
        "Binary",
        "Unary", 
        "Grouping",
        "Boolean",
        "Number",
        "String",
        "Nil",
        "Variable"
    ]

    forwardDeclareClasses(Cpp, classes)

    defineVisitableGeneric(Cpp, 2)
    defineVisitorGeneric(Cpp, classes, baseClass)

    defineBaseClass(Cpp, baseClass)
    
    for t in types:
        Cpp.insert()
        className = t.split(':')[0].strip()
        fields = t.split(':')[1].strip()
        defineType(Cpp, baseClass, className, fields)
    
    writeOut(outPath, Cpp)

def generateStmtHeaderForTypes(outDir: str, baseClass: str, types: list[str]):
    outPath = "./" + outDir + '/' + baseClass +'.h'

    Cpp = CodeAssembler()
    addTopOfFile(Cpp, baseClass, True)

    classes = [
        "Expression",
        "Print",
        "Var"
    ]

    forwardDeclareClasses(Cpp, classes)
    defineVisitableGeneric(Cpp, 1, "VisitableStmt")
    defineVisitorGeneric(Cpp, classes, baseClass, "Stmt")

    defineBaseClass(Cpp, baseClass, stmt=True)
    for t in types:
        Cpp.insert()
        className = t.split(':')[0].strip()
        fields = t.split(':')[1].strip()
        defineType(Cpp, baseClass, className, fields, stmt=True)
   
    writeOut(outPath, Cpp)

argc = len(sys.argv)
if (argc != 2):
    print("usage: py gen_ast.py <output_dir>")
    exit(64) # exit with wrong usage error

dest = sys.argv[1]
baseClass = "Expr"
types = [ 
    f"Binary    :  {baseClass}* left, Token op, {baseClass}* right",
    f"Unary     :  Token op, {baseClass}* right",
    f"Grouping  :  {baseClass}* expr",
    f"Boolean   :  bool value",
    f"Number    :  double value",
    f"String    :  string value",
    f"Nil       :",
    f"Variable  :  Token name",
]
# generateExprHeaderForTypes(dest, baseClass, types)



stmtBaseClass = "Stmt"
sTypes = [
    "Expression     :  Expr* expr",
    "Print          :  Expr* expr",
    "Var            :  Token name, Expr* initValue"
]
generateStmtHeaderForTypes(dest, stmtBaseClass, sTypes)
