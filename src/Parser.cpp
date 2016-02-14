#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

void log(std::shared_ptr<parser::Node> pNode, int depth)
{
  if (!pNode)
    return;

  for (int i = 0; i < depth; ++i)
    ::Rprintf("  ");

  ::Rprintf(toString(pNode->token()).c_str());

  for (auto&& child : pNode->children())
    log(child, depth + 1);
}

namespace {

class SEXPConverter
{
private:

  static SEXP asFunctionCallSEXP(std::shared_ptr<parser::Node> pNode)
  {
    DEBUG("asFunctionCallSEXP()");
    using namespace tokens;
    SEXP langSEXP = PROTECT(R_NilValue);
    for (auto it = pNode->children().rbegin(); it != pNode->children().rend(); ++it)
      if (!(*it)->token().isType(EMPTY))
        langSEXP = Rf_lcons(asSEXP(*it), langSEXP);
    UNPROTECT(1);
    return langSEXP;
  }

  static SEXP asFunctionDeclSEXP(std::shared_ptr<parser::Node> pNode)
  {
    return R_NilValue;
  }

public:
  static SEXP asSEXP(std::shared_ptr<parser::Node> pNode)
  {
    using namespace tokens;

    if (!pNode)
      return R_NilValue;

    auto&& token = pNode->token();

    // Handle function calls specially
    if (pNode->children().size() > 1 && (token.isType(LPAREN) || token.isType(LBRACKET) || token.isType(LDBRACKET)))
      return asFunctionCallSEXP(pNode);
    else if (token.isType(KEYWORD_FUNCTION))
      return asFunctionDeclSEXP(pNode);

    SEXP elSEXP;
    if (isOperator(token) || isSymbol(token) || isKeyword(token) || isLeftBracket(token))
      elSEXP = PROTECT(Rf_lcons(
        Rf_install(token.contents().c_str()),
        R_NilValue
      ));
    else if (isNumeric(token))
      elSEXP = PROTECT(Rf_ScalarReal(::atof(token.contents().c_str())));
    else if (isString(token))
    {
      const std::string& contents = token.contents();
      std::string substring = contents.substr(1, contents.size() - 2);
      elSEXP = PROTECT(Rf_mkString(substring.c_str()));
    }
    else
      elSEXP = PROTECT(Rf_mkString(token.contents().c_str()));

    if (pNode->children().empty())
    {
      UNPROTECT(1);
      return elSEXP;
    }

    SEXP listSEXP = PROTECT(R_NilValue);
    for (auto it = pNode->children().rbegin(); it != pNode->children().rend(); ++it)
      if (!(*it)->token().isType(EMPTY))
        listSEXP = Rf_lcons(asSEXP(*it), listSEXP);
    listSEXP = Rf_lcons(elSEXP, listSEXP);

    UNPROTECT(2);
    return listSEXP;
  }

  static SEXP asSEXP(const std::vector<std::shared_ptr<parser::Node>>& expression)
  {
    std::size_t n = expression.size();
    SEXP exprSEXP = PROTECT(Rf_allocVector(EXPRSXP, n));
    for (std::size_t i = 0; i < n; ++i)
      SET_VECTOR_ELT(exprSEXP, i, asSEXP(expression[i]));
    UNPROTECT(1);
    return exprSEXP;
  }

};

} // anonymous namespace

} // namespace sourcetools

extern "C" SEXP sourcetools_parse_string(SEXP programSEXP)
{
  const char* program = CHAR(STRING_ELT(programSEXP, 0));
  sourcetools::parser::Parser parser(program);
  auto root = parser.parse();
  for (auto&& child : root)
    sourcetools::log(child);
  return sourcetools::SEXPConverter::asSEXP(root);
}
