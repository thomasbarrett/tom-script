#include "AST/AmbiguousDecl.h"
#include "AST/AmbiguousType.h"
#include "AST/Decl.h"

AmbiguousDecl AmbiguousDecl::filter(std::shared_ptr<IdentifierExpr> e) {
  return filter([e](std::shared_ptr<Decl> d){
    return (d->as<VarDecl>() || d->as<LetDecl>()) && d->getName() == e->getLexeme();
  });
}
AmbiguousDecl AmbiguousDecl::filter(std::function<bool(std::shared_ptr<Decl>)> func) const {
  std::vector<std::shared_ptr<Decl>> filtered;
  for (auto decl: decls) {
    if (decl && func(decl)) filtered.push_back(decl);
  }
  return {filtered};
}

AmbiguousType AmbiguousDecl::getTypes() const {
  std::vector<std::shared_ptr<Type>> types;
  for (auto decl: decls) {
    types.push_back(decl->getType());
  }
  return types;
}

std::shared_ptr<Decl> AmbiguousDecl::get() const {
  if (isEmpty()) {
    throw std::logic_error("cannot access empty decl");
  } else if (isAmbiguous()) {
    throw std::logic_error("cannot access ambiguous decl");
  } else {
    return decls[0];
  }
}

std::ostream& operator<<(std::ostream& os, const AmbiguousDecl& d) {
  os << d.decls.size() << " possible declarations" << std::endl;
  for (auto decl: d.decls) {
    os << "  " << decl << std::endl;
  }
  return os;
}
