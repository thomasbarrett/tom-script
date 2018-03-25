#ifndef AST_DECL_H
#define AST_DECL_H

#include "Parse/Token.h"
#include "AST/Matchable.h"
#include "AST/DeclarationContext.h"

#include <vector>

class CompoundStmt;
class FunctionType;
class RuntimeStack;
class StackReference;

class Decl : virtual public Matchable {

public:
  enum class Kind {
    #define DECL(SELF, PARENT) SELF,
    #include "AST/Decl.def"
    #undef DECL
  };
  virtual Decl::Kind getKind() const = 0;
  virtual std::string getName() const = 0;

  std::function<void(RuntimeStack&)> func = nullptr;
  std::shared_ptr<StackReference> location;

  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }
  static std::shared_ptr<Decl> make(std::shared_ptr<Decl>, std::function<void(RuntimeStack& stack)>);
  virtual std::shared_ptr<Type> getType() const = 0;
  virtual DeclarationContext* getContext() const = 0;
  virtual void setContext(DeclarationContext*) = 0;
};

std::ostream& operator<<(std::ostream& os, Decl& x);

class DeclName : public Terminal {
public:
  Token token;
  std::string getLexeme() const;
  DeclName(Token n);
};


class TypeDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;
public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr> e);
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  TypeDecl(Token n);
};

class TypeAlias : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr> e);

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  TypeAlias(Token n, std::shared_ptr<Type> t);
};

class VarDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  VarDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};

class LetDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> expr;

  std::shared_ptr<Expr> getExpr() const;
  void setExpr(std::shared_ptr<Expr>);

  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  Decl::Kind getKind() const;
  std::string getName() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  std::shared_ptr<Type> getType() const;
  LetDecl(Token n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e);
};


class ParamDecl : public Decl, public NonTerminal {
private:
  DeclarationContext *context;

public:
  std::shared_ptr<DeclName> primary;
  std::shared_ptr<DeclName> secondary;
  std::shared_ptr<Type> type;
  std::shared_ptr<Expr> default_value;

  ParamDecl(Token p, Token s, std::shared_ptr<Type> t);
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const;
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
};

class ParamDeclList : public NonTerminal {
public:
  std::shared_ptr<ParamDecl> element;
  std::shared_ptr<ParamDeclList> list;

  ParamDeclList(std::shared_ptr<ParamDecl> e, std::shared_ptr<ParamDeclList> l);
  std::shared_ptr<TypeList> getTypeList() const;
  std::vector<std::shared_ptr<Matchable>> getChildren() const;
  void setContext(DeclarationContext* c);
};

/// A named, explicitly typed function
class FuncDecl : public Decl, public NonTerminal {
private:
  std::shared_ptr<DeclarationContext> context = std::make_shared<DeclarationContext>();

public:
  std::shared_ptr<DeclName> name;
  std::shared_ptr<ParamDeclList> params;
  std::shared_ptr<Type> returnType;
  std::shared_ptr<FunctionType> type;
  std::shared_ptr<CompoundStmt> stmt;

  FuncDecl(Token n, std::shared_ptr<ParamDeclList> t, std::shared_ptr<Type>, std::shared_ptr<CompoundStmt> s);
  Decl::Kind getKind() const;
  std::string getName() const;
  std::shared_ptr<Type> getType() const;
  DeclarationContext* getContext() const;
  void setContext(DeclarationContext* c);
  std::vector<std::shared_ptr<Matchable>> getChildren() const;

};
#endif
