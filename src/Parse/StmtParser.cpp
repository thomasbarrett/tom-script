#include "Parse/Parser.h"
#include "AST/Stmt.h"

#include <memory>


shared_ptr<Stmt> Parser::parseStmt()  {
  switch(token().getType()) {
    case Token::l_brace: return parseCompoundStmt();
    case Token::kw_if: return parseConditionalStmtList();
    case Token::kw_return: return parseReturnStmt();
    case Token::kw_while: return parseWhileLoop();
    case Token::kw_var:
    case Token::kw_let:
    case Token::kw_func:
    case Token::kw_typealias: return parseDeclStmt();
    case Token::identifier:
    case Token::integer_literal:
    case Token::double_literal:
    case Token::string_literal:
    case Token::l_paren: return parseExprStmt();
    default: throw CompilerException(token().getLocation(), "expected statement but found " + token().lexeme);
  }
}

shared_ptr<CompoundStmt> Parser::parseCompoundStmt()  {
  expectToken(Token::l_brace, "left brace");
  while(token().is(Token::new_line)) consume();
  if (consumeToken(Token::r_brace)) return std::make_shared<CompoundStmt>(std::vector<std::shared_ptr<Stmt>>());
  auto list = parseStmtList();
  while(token().is(Token::new_line)) consume();
  expectToken(Token::r_brace, "right brace");
  return std::make_shared<CompoundStmt>(std::move(list));
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

std::vector<std::shared_ptr<Stmt>> Parser::parseStmtList()  {
  std::vector<std::shared_ptr<Stmt>> elements;
  while(token().is(Token::new_line)) consume();
  if (token().isAny({Token::r_brace, Token::eof})) return elements;
  while(token().is(Token::new_line)) consume();
  elements.push_back(parseStmt());
  while(token().is(Token::new_line)) consume();
  while (token().isNot(Token::r_brace) && token().isNot(Token::eof)) {
    elements.push_back(parseStmt());
    while(token().is(Token::new_line)) consume();
  }
  return elements;
}

shared_ptr<WhileLoop> Parser::parseWhileLoop()  {
  expectToken(Token::kw_while, "while");
  auto expr = parseExpr();
  auto stmt = parseCompoundStmt();
  return std::make_shared<WhileLoop>(expr, stmt);
}

shared_ptr<ReturnStmt> Parser::parseReturnStmt() {
  expectToken(Token::kw_return, "return");
  if (consumeToken(Token::new_line)) return std::make_shared<ReturnStmt>(nullptr);
  auto expr = parseExpr();
  expectToken(Token::new_line, "new line");
  return std::make_shared<ReturnStmt>(expr);
}
