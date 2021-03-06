import sys
from datetime import datetime

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

sclasses = [
        "Expression",
        "Print",
        "Var",
        "Block",
        "If",
        "While",
        "Function",
        "Return", 
        "Class"
    ]

eclasses = [
        "Assign",
        "Binary",
        "Unary", 
        "Grouping",
        "Boolean",
        "Number",
        "String",
        "Nil",
        "Variable",
        "Logical",
        "Call",
        "Get",
        "Set",
        "This",
        "Super",
        # "Lambda" # Callable Expr type
    ]

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
        "Block": '{',
        "If" : 'i',
        "Logical": 'L',
        "While": 'W',
        "Call": 'C',
        "Function": 'F',
        "Return": 'R',
        "Class": 'c',
        "Get": 'g',
        "Set": 'S',
        "This": 'T',
        "Super": 'p',
        # "Lambda": 'l'
    }

# add top comments and include statements
# aka boilerplate
def addTopOfFile(Cpp: CodeAssembler, baseClass: str, stmt=False):
    Cpp.insert("//") 
    Cpp.insert(f"// {baseClass}.h")
    Cpp.insert("// Croix")
    Cpp.insert("//")
    day = datetime.today().strftime('%Y-%m-%d')
    Cpp.insert(f"// Auto-generated by Joshua Pepple on {day}.")
    Cpp.insert(f"// CAUTION: Do not hand edit! Edit gen_ast.py instead.")
    Cpp.insert("//")
    Cpp.insert()
    Cpp.insert("#pragma once")
    Cpp.insert()

    Cpp.insert("#include <iostream>")
    Cpp.insert("#include <string>")
    Cpp.insert('#include "Token.h"')
    if stmt:
        Cpp.insert('#include "Expr.h"')
        Cpp.insert("#include <vector>")
    # else:
    #     Cpp.insert('#include "Stmt.h"')
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
        returns = ["string", "Storable *", "void"]

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

    if stmt:
        Cpp.insert(f"class {baseClass} : {inher} " + "{")
    else:
        Cpp.insert(f"class {baseClass} : {inher}" + ", public Storable {")
    Cpp.insert("public:")
    Cpp.indentInsertDedent("virtual char type() const = 0;")
    if not stmt:
        Cpp.insert();
        Cpp.indentInsertDedent("string storedType() {")
        Cpp.indent()
        Cpp.indentInsertDedent('return "Expr";')
        Cpp.dedent()
        Cpp.indentInsertDedent("}")
    Cpp.insert();
    Cpp.indentInsertDedent(f"virtual ~{baseClass}() " + "{ }")
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
            varName = ''
            if "> " in f:
                varName = f.split("> ")[1].strip()
            else:
                varName = f.split(" ")[1].strip()

            Cpp.indentInsertDedent(f"this->{varName} = {varName};")
    Cpp.insert("}")

    Cpp.insert()
    # destructor
    Cpp.insert(f"~{className}() " + "{")
    # destructor body
    for f in fields:
        if '*' in f and '>' not in f: # is a pointer, delete it
            # print(f"{f} is a pointer")
            member = f.split('*')[1].strip()
            Cpp.indentInsertDedent(f"delete this->{member};")
        if 'vector' in f and '*' in f: # a vector of pointers, delete them
            vec = f.split(">")[1].strip()
            Cpp.indentInsertDedent(f"for(int i = 0; i < {vec}.size(); ++i) " + "{")
            Cpp.indent()
            Cpp.indentInsertDedent(f"delete {vec}[i];")
            Cpp.dedent()
            Cpp.indentInsertDedent("}")
            # print(vec)
    Cpp.insert("}")

    # generate necessary accept functions
    returns : list[str]

    if stmt:
        returns = ["void"]
    else:
        returns = ["string", "Storable *", "void"]
    
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

def defineStorableInterface(Cpp: CodeAssembler):
    Cpp.insert()
    Cpp.insert("// used by Environment to store both Exprs")
    Cpp.insert("// and Callables like native functions and")
    Cpp.insert("// user defined functions")
    Cpp.insert()
    Cpp.insert("class Storable {") 
    Cpp.insert("public:")
    Cpp.indentInsertDedent("virtual string storedType() = 0;")
    Cpp.insert("};")

def writeOut(path: str, Cpp: CodeAssembler):
    print("writing to", path)
    with open(path, 'w+') as astFile:
        astFile.write(str(Cpp))

def generateExprHeaderForTypes(outDir: str, baseClass: str, types: list[str]):
    outPath = "./" + outDir + '/' + baseClass +'.h'

    Cpp = CodeAssembler()
    addTopOfFile(Cpp, baseClass)    

    forwardDeclareClasses(Cpp, eclasses)

    # ASTPrinter, Interpreter, Resolver
    defineVisitableGeneric(Cpp, 3)
    defineVisitorGeneric(Cpp, eclasses, baseClass)
    defineStorableInterface(Cpp)

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

    forwardDeclareClasses(Cpp, sclasses)
    defineVisitableGeneric(Cpp, 1, "VisitableStmt")
    defineVisitorGeneric(Cpp, sclasses, baseClass, "Stmt")

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
    f"Assign       :  Token name, {baseClass}* value",
    f"Binary       :  {baseClass}* left, Token op, {baseClass}* right",
    f"Unary        :  Token op, {baseClass}* right",
    f"Grouping     :  {baseClass}* expr",
    f"Boolean      :  bool value",
    f"Number       :  double value",
    f"String       :  string value",
    f"Nil          :",
    f"Variable     :  Token name",
    f"Logical      : {baseClass}* left, Token op, {baseClass}* right",
    f"Call         : Expr* callee, Token rParen, vector < Expr* > arguments",
    f"Get          : Expr* object, Token name",
    f"Set          : Expr* object, Token name, Expr* value",
    f"This         : Token keyword",
    f"Super        : Token keyword, Token property",
    # "Lambda        :  vector < Token > params, Block* body"
]
generateExprHeaderForTypes(dest, baseClass, types)



stmtBaseClass = "Stmt"
sTypes = [
    "Expression     :  Expr* expr",
    "Print          :  Expr* expr",
    "Var            :  Token name, Expr* initValue",
    "Block          :  vector < Stmt* > stmts",
    "If             :  Expr* cond, Stmt* then, Stmt* else_",
    "While          :  Expr* cond, Stmt* body",
    "Function       :  Token fnName, vector < Token > params, Block* body",
    "Return         :  Token ret, Expr* value",
    "Class          :  Token name, Variable* superclass, vector < Function* > methods",
]
# generateStmtHeaderForTypes(dest, stmtBaseClass, sTypes)
