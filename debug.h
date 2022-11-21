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
    void visit_conditional_statement(ConditionalStatement *node, int visit_count) override;
    void visit_while_statement(WhileStatement *node, int visit_count) override;
    void visit_epsilon_statement(EpsilonStatement *node, int visit_count) override;
    void visit_number_expression_1(NumberExpression1 *node, int visit_count) override;
    void visit_number_expression_2(NumberExpression2 *node, int visit_count) override;
    void visit_number_expression_3(NumberExpression3 *node, int visit_count) override;
    void visit_number_expression_4(NumberExpression4 *node, int visit_count) override;
    void visit_number_expression_5(NumberExpression5 *node, int visit_count) override;
    void visit_boolean_expression_1(BooleanExpression1 *node, int visit_count) override;
    void visit_boolean_expression_2(BooleanExpression2 *node, int visit_count) override;
    void visit_boolean_expression_3(BooleanExpression3 *node, int visit_count) override;
    void visit_boolean_expression_4(BooleanExpression4 *node, int visit_count) override;
    void visit_variable_expression(VariableExpression *node, int visit_count) override;
    void visit_value_expression(ValueExpression *node, int visit_count) override;
    void visit_boolean_value_expression(BooleanValueExpression *node, int visit_count) override;

    int m_depth{0};
};

#endif //MIMA_COMPILER_DEBUG_H
