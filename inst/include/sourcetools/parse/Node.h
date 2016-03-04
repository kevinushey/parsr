#ifndef SOURCE_TOOLS_PARSE_NODE_H
#define SOURCE_TOOLS_PARSE_NODE_H

#include <memory>

#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {

class Node
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef std::vector<Node*> Children;

private:
  Token token_;
  Node* parent_;
  Children children_;

public:

  explicit Node(const Token& token)
    : token_(token), parent_(NULL)
  {
  }

  explicit Node(const TokenType& type)
    : token_(Token(type)), parent_(NULL)
  {
  }

  static Node* create(const Token& token)
  {
    return new Node(token);
  }

  static Node* create(const TokenType& type)
  {
    return new Node(type);
  }

  static void destroy(const Node* pNode)
  {
    for (Children::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      destroy(*it);
    }

    delete pNode;
  }

  void remove(const Node* pNode)
  {
    children_.erase(
      std::remove(children_.begin(), children_.end(), pNode),
      children_.end());
  }

  void add(Node* pNode)
  {
    if (pNode->parent())
      pNode->parent_->remove(pNode);

    pNode->parent_ = this;
    children_.push_back(pNode);
  }

  const Token& token() const { return token_; }
  const Node* parent() const { return parent_; }
  const Children& children() const { return children_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_NODE_H */