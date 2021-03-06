#ifndef AST_EXPR_H
#define AST_EXPR_H

#include <memory>

#include "Basic/Token.h"
#include "Basic/SourceCode.h"

#include "AST/Type.h"
#include "AST/TreeElement.h"


class Decl;

class Expr : public TreeElement {
private:
  /*
   * The static type of the expression. This should be created at construction
   * in all cases. A public accessor is available to get the type, and a
   * protected setter is available to set the type at construction time.
   */
    class Type *type_ = nullptr;

  /*
   * The location in text where this expression was parsed from. This is useful
   * for printing debug information. It can be accessed with the public
   * getLocation method.
   */
  SourceRange fRange;
protected:
  /**
   * Expr Subclasses should call this in their constructor. This sets the
   * location in text where the expression was parsed from so that
   * meaningful error messages can be displayed.
   */
  void setSourceRange(const SourceRange& aRange) {
    fRange = aRange;
  };

public:
  /**
   * Expr Subclasses should call this in their constructor. After construction,
   * all expressions should have a type.
   */
  void setType( class Type *aType) {
    type_ = aType;
  };

  virtual std::string name() const override {
    return "expr";
  };

  /**
   * An enumeration allowing for switching over the dynamic runtime type of an
   * Expression. Very useful when working with polymorphic types.
   */
  enum class Kind {
    #define EXPR(SELF, PARENT) SELF,
    #include "AST/Expr.def"
    #undef EXPR
  };

  virtual const char* location() const = 0;

  // Virtual destructor in base class ensures safe leak-free destruction
  virtual ~Expr() = default;

  /**
   * Convenience method for checking the runtime type of an Expression.
   * Returns true if conversion to derived type is possible. Otherwise
   * return false.
   */
   template<typename T> bool is() const {
     return (dynamic_cast<const T*>(this) != nullptr);
   }

   template<typename T> bool isType() const {
     assert(type_ && "may only be called after type deduction");
     return type_ && dynamic_cast<const T*>(type_->getCanonicalType());
   }

   template<typename T> bool isReferenceTo() const {
      if (const ReferenceType *ref_type = type_->as<ReferenceType>()) {
        return ref_type->getReferencedType()->getCanonicalType()->is<T>();
      } else return false;
    }

   /**
    * Convenience method for casting Expr base type to any one of its derived
    * types. Throws a std::logic_error if conversion is not possible.
    */
  template<typename T> T* as() {
    return dynamic_cast<T*>(this);
  }

  /**
   * All derived type should be able to answer whether or not they are a left
   * value (lvalue). A lvalue is any value which can appear on the left hand
   * side of an equal sign. If an expression returns true, it means that it
   * is able to be assigned to. If not, this method returns false;
   */
  virtual bool isLeftValue() const = 0;

  /**
   * This method returns the runtime type of the expression. All derived types
   * should override this method.
   */
  virtual Expr::Kind getKind() const = 0;

  /**
   * Returns the location in input text where this expression was parsed from.
   */
  SourceRange getSourceRange() const {
    return fRange;
  }



  /**
   * Returns the type of the expression. This should never return null, as all
   * expression derived classes are required to set their type in their
   * constructor.
   */
  class Type* getType() const {
    return type_;
  };

   class Type* type() const {
    return type_;
  };
};



class IntegerExpr: public Expr  {
private:
  Token token_;

public:

  IntegerExpr(Token t) : token_{t} {
    if (t.isNot(Token::integer_literal)) {
      throw std::domain_error("IntegerExpr requires a token of type integer_literal");
    }
  }

  StringRef lexeme() const {
    return token_.lexeme();
  }

  const char* location() const override {
      return token_.location();
  }

  std::string name() const override {
    return "integer-literal-expression";
  };

  Expr::Kind getKind() const override { return Kind::IntegerExpr; }

  int getInt() const {
    return std::stoi(token_.lexeme().str());
  }

  bool isLeftValue() const override {
    return false;
  }


};


class BoolExpr: public Expr {
private:
  Token token_;

public:

  /* Returns a vector of children for easy traversal */
  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind  getKind() const override { return Kind::BoolExpr; }

  bool getBool() const {
    return token_.lexeme() == "true" ? 1: 0;
  }

  const char* location() const override {
      return token_.location();
  }


  std::string name() const override {
    return "boolean-literal-expression";
  };

  bool isLeftValue() const override {
    return false;
  }

  BoolExpr(Token t) : token_{t} {
    if (!(t.isAny({Token::kw_true, Token::kw_false}))) {
      throw std::domain_error("BoolExpr requires a boolean literal");
    }
  }

};

class CharacterExpr: public Expr {
private:
  Token token_;

public:

  /* Returns a vector of children for easy traversal */
  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::CharacterExpr; }

  bool isLeftValue() const override {
    return false;
  }

  const char* location() const override {
      return token_.location();
  }

  std::string name() const override {
    return "character-literal-expression";
  };

  double getChar() const {
    char first = *(token_.lexeme().start + 1);
    if (first == '\\') {
      char second = *(token_.lexeme().start + 2);
      switch (second) {
        case 'n': return '\n';
        case '0': return '\0';
        default: throw std::logic_error("this escape not supported");
      }
    } else return first;
  }

  CharacterExpr(Token t) : token_{t} {
    if (t.isNot(Token::character_literal)) {
      throw std::domain_error("CharacterExpr requires a token of type character_literal");
    }
  }

};


class DoubleExpr: public Expr {
private:
  Token token_;

public:

  /* Returns a vector of children for easy traversal */
  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::DoubleExpr; }

  bool isLeftValue() const override {
    return false;
  }

  std::string name() const override {
    return "double-literal-expression";
  };

  const char* location() const override {
      return token_.location();
  }

  double getDouble() const {
    return std::stod(token_.lexeme().str());
  }

  DoubleExpr(Token t) : token_{t} {
    if (t.isNot(Token::double_literal)) {
      throw std::domain_error("DoubleExpr requires a token of type double_literal");
    }
  }

};


class IdentifierExpr: public Expr {
private:
  Token token_;
  const Decl* decl_;
public:

  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::IdentifierExpr; }

  void setDecl(const Decl* decl) {
    decl_ = decl;
  }

  const char* location() const override {
      return token_.location();
  }

  const Decl* getDecl() const {
    return decl_;
  }

  bool isLeftValue() const override;

  IdentifierExpr(Token tok) : token_{tok} {}

  std::string name() const override {
    return "identifier-expression";
  };

};


/**
 * An Expr subclass that represents a unary expression. Composed of an Expr and
 * an OperatorExpr. All members are guarenteed to be non-null.
 *
 * <UnaryExpr> ::= <OperatorExpr> <Expr>
 */
class UnaryExpr: public Expr {
private:
  Token op_;
  std::unique_ptr<Expr> expr_;
public:

  std::vector<TreeElement*> getChildren() const override {
    return {expr_.get()};
  }

  Expr::Kind getKind() const override { return Kind::UnaryExpr; }

  bool isLeftValue() const override;

  const char* location() const override {
      return op_.location();
  }

  UnaryExpr(Token o, std::unique_ptr<Expr> e) : op_{std::move(o)}, expr_{std::move(e)} {
    if (!expr_) {
      throw std::domain_error("BinaryExpr: expr is required");
    }
  }

  std::string name() const override {
    return "unary-expression";
  };

  StringRef getOperator() const {
    return op_.lexeme();
  }

  const Expr& getExpr() const {
    return *expr_;
  }

  Expr& getExpr() {
    return *expr_;
  }


};

/**
 * An Expr subclass that represents a binary expression. Composed of a left and
 * right Expr and an OperatorExpr specifying the operation. All members are
 * guarenteed to be non-null.
 *
 * <BinaryExpr> ::= <Expr> <OperatorExpr> <Expr>
 */
class BinaryExpr: public Expr {
private:
  std::unique_ptr<Expr> left_;
  Token op_;
  std::unique_ptr<Expr> right_;
public:

  Expr::Kind getKind() const override { return Kind::BinaryExpr; }

  std::vector<TreeElement*> getChildren() const override {
    return {left_.get(), right_.get()};
  }

  bool isLeftValue() const override {
    return false;
  }

  const char* location() const override {
      return op_.location();
  }

  std::string name() const override {
    return "binary-expression";
  };

  BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
  : left_{std::move(l)}, op_{o}, right_{std::move(r)} {
    if (!left_) {
      throw std::domain_error("BinaryExpr: left is required");
    }

    if (!op_.is(Token::operator_id)) {
      throw std::domain_error("BinaryExpr: invalid operator");
    }

    if (!right_) {
      throw std::domain_error("BinaryExpr: right is required");
    }
  }


  const Expr& getLeft() const {
    return *left_;
  }

  const Expr& getRight() const {
    return *right_;
  }
  Expr& getLeft() {
    return *left_;
  }

  Expr& getRight() {
    return *right_;
  }
  StringRef getOperator() const {
    return op_.lexeme();
  }

};

class FunctionCall: public Expr {
private:
  std::unique_ptr<IdentifierExpr> name_;
  std::vector<std::unique_ptr<Expr>> arguments_;
public:

  FunctionCall(std::unique_ptr<IdentifierExpr> n, std::vector<std::unique_ptr<Expr>> a)
  : name_{std::move(n)}, arguments_{std::move(a)} {
  }

  bool isLeftValue() const override {
    return false;
  }

  Expr::Kind getKind() const override { return Kind::FunctionCall; }

  const char* location() const override {
      return name_->location();
  }

  std::string name() const override {
    return "function-call-expression";
  };

  std::vector<TreeElement*> getChildren() const override {
    std::vector<TreeElement*> children;
    for (auto &arg: arguments_) {
      children.push_back(arg.get());
    }
    return children;
  }
  const std::vector<std::unique_ptr<Expr>>& getArguments() const {
    return arguments_;
  }
  StringRef getFunctionName() const {
    return name_->lexeme();
  }

};

class ListExpr: public Expr {
private:
  std::vector<std::unique_ptr<Expr>> elements_;

public:

  Expr::Kind getKind() const override {
    return Kind::ListExpr;
  }
  bool isLeftValue() const override {
    return false;
  }

  ListExpr(std::vector<std::unique_ptr<Expr>> d): elements_{std::move(d)} {}

  const char* location() const override {
      return elements_[0]->location();
  }

  std::string name() const override {
    return "list-expression";
  };

  const std::vector<std::unique_ptr<Expr>>& elements() const {
    return elements_;
  }

  std::vector<std::unique_ptr<Expr>>& elements() {
    return elements_;
  }
};


class StringExpr: public Expr {
private:
  Token token_;

public:

  StringRef lexeme() const {
    return token_.lexeme();
  }

  Expr::Kind getKind() const override { return Kind::StringExpr; }

  bool isLeftValue() const override {
    return false;
  }

  const char* location() const override {
      return token_.location();
  }

  std::string getString() const {
    std::string str = token_.lexeme().str();
    return str.substr(1, str.size()-2);
  }

  std::string name() const override {
    return "string-literal-expression";
  };

  StringExpr(Token t) : token_{t} {
    if (t.isNot(Token::string_literal)) {
      throw std::domain_error("StringExpr requires a token of type string_literal");
    }
  }
};

class AccessorExpr: public Expr {
private:
  std::unique_ptr<Expr> aggregate_;
  std::unique_ptr<Expr> index_;
  int member_index_;

public:

  std::vector<TreeElement*> getChildren() const override {
    return {aggregate_.get(), index_.get()};
  }

  Expr::Kind getKind() const override {
    return Expr::Kind::AccessorExpr;
  }

  const char* location() const override {
      return aggregate_->location();
  }

  void setMemberIndex(int i) {
    member_index_ = i;
  }

  int getMemberIndex() const {
    if (IntegerExpr* int_expr = dynamic_cast<IntegerExpr*>(index_.get())) {
      return int_expr->getInt();
    } else return member_index_;
  }

  bool hasStaticIndex() const {
    return index_->is<IdentifierExpr>() || index_->is<IntegerExpr>();
  }

  bool isLeftValue() const override {
    return true;
  }

  virtual std::string name() const override {
    return "accessor-expression";
  };

  const Expr& identifier() const {
    return *aggregate_;
  }

  Expr& identifier() {
    return *aggregate_;
  }

  Expr& index() const {
    return *index_;
  }

  AccessorExpr(std::unique_ptr<Expr> a, std::unique_ptr<Expr> b): aggregate_{std::move(a)}, index_{std::move(b)} {}

};


class TupleExpr: public Expr {
private:
  std::vector<std::unique_ptr<Expr>> elements_;

public:

  /* Returns a vector of children for easy traversal */
  Expr::Kind getKind() const override { return Kind::TupleExpr; }

  bool isLeftValue() const override{
    return false;
  }

  const char* location() const override {
      return elements_[0]->location();
  }

  int size() const { return elements_.size(); }

  Expr& operator[] (int x) {
    return *elements_[x];
  }

  std::string name() const override {
    return "tuple-expression";
  };

  const std::vector<std::unique_ptr<Expr>>& elements() const {
    return elements_;
  }

  TupleExpr(std::vector<std::unique_ptr<Expr>> list) : elements_{std::move(list)} {
  }

};


#endif
