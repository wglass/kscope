#include "ast.h"
#include "tree.h"

#include "lexer.h"
#include "bison_parser.hh"


STree::STree() : root(nullptr) {}

STree::STree(STree &&other) {
    root = std::move(other.root);
    other.root = nullptr;
}

STree &
STree::operator =(STree other) {
    std::swap(root, other.root);
    return *this;
}

STree::~STree() {
    root.reset();
}

void
STree::parse(std::istream &input) {
    root.release();

    Lexer lexer = Lexer(&input);
    bison::Parser *parser = new bison::Parser(lexer,
                                              *this);

    parser->parse();
}

void
STree::set_root(FunctionNode *node) {
    root = unique_ptr<ASTNode>(node);
}

void
STree::set_root(PrototypeNode *node) {
    root = unique_ptr<ASTNode>(node);
}
