/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp4: You will need to extend the Decl classes to implement 
 * code generation for declarations.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"

class Type;
class NamedType;
class Identifier;
class Stmt;
class SymbolTable;

class Decl : public Node 
{
  protected:
    Identifier *id;
    SymbolTable *scope;
    int offset;
    Location * loc;
    char *label;
  
  public:
    Decl(Identifier *name);
    Decl() {};
    virtual void BuildClass() {}
    const char * GetName() { return id->GetName(); }
    virtual Type* GetType() { return NULL; };
    virtual Location* GetLoc() { return loc; };
    virtual void SetLoc(int location, bool func) {};
    virtual void AddSymbols() {};
    virtual Decl* SearchMembers(char *name) { return NULL; };
    virtual List<Decl*>* GetMemberVars() { return NULL; }
    virtual List<Decl*>* GetMemberFunc() { return NULL; }
    virtual int GetFunctionOffset(char*) { return -1; }
    void SetLabel(char* newLabel) { label = strdup(newLabel); }
    char* GetLabel() { return label; }
    Decl * SearchScope(char * name); 
    int GetOffset() { return offset; }
    void SetOffset(int newOffset) { offset = newOffset; }
    friend ostream& operator<<(ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    //returns the type of the ident
    Type * GetType() { return type; }
    VarDecl(Identifier *name, Type *type);
    VarDecl(const char *name, Type *type);
    //Used to set the location of this variable in memeory to a new Location object
    // offset is the offset from the program start untill this variable is defined
    void SetLoc(int location, bool func);
    void AddSymbols();

    // returns the size in bytes of the object
    Location* Emit(CodeGenerator* codeGen);
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    List<Decl*> *memberVars;
    List<Decl*> *memberFuncs;
    List<const char*> *functions;
    Hashtable<int> *funcOffsets;
    bool emitted;
    bool built;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    void BuildClass(); 
    Type* GetType();
    void AddSymbols();
    List<Decl*>* GetMemberVars() { return memberVars; }
    List<Decl*>* GetMemberFunc() { return memberFuncs; }
    char* GetLabel() { return NULL; }
    Location* Emit(CodeGenerator* codeGen);
    Location* GetLoc() { return loc; };
    Decl* SearchMembers(char *name);
    int GetFunctionOffset(char *name) { return funcOffsets->Lookup(name); }
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    Location* Emit(CodeGenerator* codeGen);
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    
  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    Type* GetType() { return returnType; };
    Location* Emit(CodeGenerator* codeGen);
    void AddSymbols(); 
    Decl* SearchFormals(char *name);
};

#endif
