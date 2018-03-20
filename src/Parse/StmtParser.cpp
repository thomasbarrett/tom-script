#include "Parse/Parser.h"
#include "AST/Stmt.h"

#include <memory>

shared_ptr<Stmt> Parser::makeStmt(std::string text)  {
  const std::stringstream sstream{text};
  auto source = SourceCode{sstream, "factory"};
  auto parser = Parser{&source};
  try {
    auto stmt = parser.parseStmt();
    return stmt;
  } catch (std::string) {
    return nullptr;
  }
}

shared_ptr<Stmt> Parser::parseStmt()  {
  while(token().is(Token::new_line)) consume();

  switch(token().getType()) {
    case Token::l_brace: return parseCompoundStmt();
    case Token::kw_if: return parseConditionalStmtList();
    case Token::kw_return: return parseReturnStmt();
    case Token::kw_while: return parseWhileLoop();
    case Token::kw_var:
    case Token::kw_let:
    case Token::kw_func:
    case Token::kw_typealias: return parseDeclStmt();
    default: return parseExprStmt();
  }
}

shared_ptr<CompoundStmt> Parser::parseCompoundStmt()  {
  expectToken(Token::l_brace, "left brace");
  while(token().is(Token::new_line)) consume();
  if (consumeToken(Token::r_brace)) return std::make_shared<CompoundStmt>(nullptr);
  auto list = parseStmtList();
  expectToken(Token::r_brace, "right brace");
  return std::make_shared<CompoundStmt>(list);
}

shared_ptr<ConditionalStmt> Parser::parseConditionalStmt() {
  auto expr = parseExpr();
  auto stmt = parseCompoundStmt();
  return std::make_shared<ConditionalStmt>(expr, stmt);
}

shared_ptr<ConditionalStmtList> Parser::parseConditionalStmtList()  {

  if (consumeToken(Token::kw_if)) {
    auto stmt = parseConditionalStmt();
    if (consumeToken(Token::kw_else)) {
      auto list = parseConditionalStmtList();
      return std::make_shared<ConditionalStmtList>(stmt, list);
    } else {
      return std::make_shared<ConditionalStmtList>(stmt, nullptr);
    }
  } else {
    auto stmt = parseCompoundStmt();
    return std::make_shared<ConditionalStmtList>(std::make_shared<ConditionalStmt>(nullptr, stmt), nullptr);
  }
}

shared_ptr<DeclStmt> Parser::parseDeclStmt()  {
  auto decl = parseDecl();
  expectToken(Token::new_line, "new line");
  return std::make_shared<DeclStmt>(decl);
}

shared_ptr<ExprStmt> Parser::parseExprStmt() {
  auto expr = parseExpr();
  expectToken(Token::new_line, "new line");
  return std::make_shared<ExprStmt>(expr);
}

shared_ptr<StmtList> Parser::parseStmtList()  {
  auto stmt = parseStmt();
  while(token().is(Token::new_line)) consume();
  if (!acceptToken(Token::eof) && !acceptToken(Token::r_brace)) {
    auto list = parseStmtList();
    return std::make_shared<StmtList>(stmt, list);
  }
  return std::make_shared<StmtList>(stmt, nullptr);
}

shared_ptr<WhileLoop> Parser::parseWhileLoop()  {
  expectToken(Token::kw_while, "while");
  auto expr = parseExpr();
  auto stmt = parseCompoundStmt();
  return std::make_shared<WhileLoop>(expr, stmt);
}

shared_ptr<ReturnStmt> Parser::parseReturnStmt() {
  expectToken(Token::kw_return, "return");
  auto expr = parseExpr();
  return std::make_shared<ReturnStmt>(expr);
}
