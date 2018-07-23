#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "Parse/Scope.h"
#include "AST/Type.h"

#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"


TEST(DeclParser, parseLetDecl) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseLetDecl();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5"));
}

TEST(StmtParser, parseDeclStmt) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseDeclStmt();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5\n"));
}

TEST(StmtParser, parseStmtList) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseStmtList();
  };

  EXPECT_NO_THROW(parse("let a: Integer = 5\nreturn a\n"));
}

TEST(StmtParser, parseCompoundStmt) {

  auto parse = [](std::string text) {
    SourceManager::currentSource = new SourceCode(std::istringstream(text),"test");
    Parser parser = Parser{SourceManager::currentSource};
    return parser.parseCompoundStmt();
  };

  EXPECT_NO_THROW(parse("{\nlet a: Integer = 5\nreturn a\n}"));
}
