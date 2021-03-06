#include <gtest/gtest.h>
#include "Basic/SourceCode.h"
#include "AST/Type.h"
#include "Basic/CompilerException.h"
#include "Parse/Parser.h"
#include "Parse/Operator.h"

TEST(ExprParser, parseIntegerExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseIntegerExpr();
  };

  EXPECT_EQ(parse("123")->getInt(), 123);
}

TEST(ExprParser, parseIdentifierExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseIdentifier();
  };

  EXPECT_NO_THROW(parse("abc"));
}


TEST(ExprParser, parseDoubleExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseDoubleExpr();
  };

  EXPECT_EQ(parse("123.456")->getDouble(), 123.456);

}


TEST(ExprParser, parseValueExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseValueExpr();
  };

  EXPECT_TRUE(parse("123.456") != nullptr);
  EXPECT_TRUE(parse("123") != nullptr);
  EXPECT_TRUE(parse("abc") != nullptr);
}

TEST(ExprParser, parseUnaryExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseUnaryExpr();
  };

  EXPECT_NO_THROW(parse("+1"));
  EXPECT_NO_THROW(parse("-1"));
  EXPECT_NO_THROW(parse("+1.0"));
  EXPECT_NO_THROW(parse("-5.0"));

}

TEST(ExprParser, parseBinaryExpr) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseBinaryExpr(OperatorTable::size());
  };

  EXPECT_NO_THROW(parse("1+1"));
  EXPECT_NO_THROW(parse("4-abc"));
}

TEST(ExprParser, parseExprList) {

  auto parse = [](std::string text) {
    std::stringstream ss{text};
    std::shared_ptr<SourceFile> src = std::make_shared<SourceFile>(ss);
    Parser parser = Parser{src};
    return parser.parseExprList();
  };

  EXPECT_EQ(parse("1,2,3,4,5,6").size(), 6);

}
