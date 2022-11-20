#ifndef MIMA_COMPILER_DEBUG_H
#define MIMA_COMPILER_DEBUG_H

#include "ast.h"

class PrinterNodeVisitor : public NodeVisitor {
public:
    void print_tree(Node *node) { node->visit(*this); };

private:
    void visit_var_statement(VarStatement *node, int visit_count) override;
    void visit_assignment_statement(AssignmentStatement *node, int visit_count) override;
    void visit_origin_statement(OriginStatement *node, int visit_count) override;
    void visit_epsilon_statement(EpsilonStatement *node, int visit_count) override;
    void visit_number_expression_1(NumberExpression1 *node, int visit_count) override;
    void visit_number_expression_2(NumberExpression2 *node, int visit_count) override;
    void visit_number_expression_3(NumberExpression3 *node, int visit_count) override;
    void visit_variable(Variable *node, int visit_count) override;
    void visit_number(Number *node, int visit_count) override;

    int m_depth{0};
};

#endif //MIMA_COMPILER_DEBUG_H
