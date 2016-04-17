#include "ast/ASTNode.h"
#include "ast/FunctionNode.h"
#include "ast/PrototypeNode.h"

#include "ASTree.h"

#include "lexer.h"
#include "bison_parser.hh"


ASTree::ASTree() : root(nullptr) {}

ASTree::ASTree(ASTree &&other) {
    root = std::move(other.root);
    other.root = nullptr;
}

ASTree &
ASTree::operator =(ASTree other) {
    std::swap(root, other.root);
    return *this;
}

ASTree::~ASTree() {
    root.reset();
}

void
ASTree::parse(std::istream &input) {
    root.release();

    Lexer lexer = Lexer(&input);
    bison::Parser *parser = new bison::Parser(lexer,
                                              *this);

    parser->parse();
}

void
ASTree::set_root(FunctionNode *node) {
    root = std::unique_ptr<ASTNode>(node);
}

void
ASTree::set_root(PrototypeNode *node) {
    root = std::unique_ptr<ASTNode>(node);
}
