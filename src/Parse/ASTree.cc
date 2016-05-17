#include "kscope/AST/ASTree.h"

#include "kscope/AST/ASTNodes.h"

#include "lexer.h"
#include "bison_parser.hh"

#include <memory>
#include <iostream>


ASTree::ASTree() : root(nullptr) {}

ASTree::ASTree(ASTree &&other) {
    root = std::move(other.root);
}

ASTree &
ASTree::operator =(ASTree other) {
    std::swap(root, other.root);
    return *this;
}

ASTree::~ASTree() {
    root.release();
}

void
ASTree::parse(std::istream &input_stream) {
    root.release();

    auto lex = std::make_unique<Lexer>(&input_stream);

    bison::Parser *parser = new bison::Parser(*lex,
                                              *this);
    parser->parse();
}

void
ASTree::set_root(ASTNode *node) {
  root = std::unique_ptr<ASTNode>(node);
}
