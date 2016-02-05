#include <tests/testthat.h>
#include <sourcetools.h>

context("Tokenizer") {

  test_that("Complements are detected correctly") {

    using namespace sourcetools::tokens;

    expect_true(utils::complement(LPAREN)    == RPAREN);
    expect_true(utils::complement(LBRACE)    == RBRACE);
    expect_true(utils::complement(LBRACKET)  == RBRACKET);
    expect_true(utils::complement(LDBRACKET) == RDBRACKET);

    expect_true(utils::complement(RPAREN)    == LPAREN);
    expect_true(utils::complement(RBRACE)    == LBRACE);
    expect_true(utils::complement(RBRACKET)  == LBRACKET);
    expect_true(utils::complement(RDBRACKET) == LDBRACKET);

    expect_true(utils::isComplement(LPAREN,    RPAREN));
    expect_true(utils::isComplement(LBRACE,    RBRACE));
    expect_true(utils::isComplement(LBRACKET,  RBRACKET));
    expect_true(utils::isComplement(LDBRACKET, RDBRACKET));

    expect_true(utils::isComplement(RPAREN,    LPAREN));
    expect_true(utils::isComplement(RBRACE,    LBRACE));
    expect_true(utils::isComplement(RBRACKET,  LBRACKET));
    expect_true(utils::isComplement(RDBRACKET, LDBRACKET));
  }

  test_that("Keywords are detected correctly") {
    std::string code = "if for while break repeat";
    const auto& tokens = sourcetools::tokenize(code);
    for (auto& token : tokens) {
      if (sourcetools::tokens::utils::isWhitespace(token))
        continue;
      expect_true(sourcetools::tokens::utils::isKeyword(token));
    }
  }

  test_that("Various type-checkers work correctly") {

    using namespace sourcetools::tokens::utils;
    using namespace sourcetools::tokens;

    for (auto type : {LPAREN, LBRACE, LBRACKET, LDBRACKET,
                      RPAREN, RBRACE, RBRACKET, RDBRACKET})
    {
      expect_true(isBracket(Token(type)));
    }
  }

}
