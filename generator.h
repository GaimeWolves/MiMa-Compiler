#ifndef MIMA_COMPILER_GENERATOR_H
#define MIMA_COMPILER_GENERATOR_H

#include <string>
#include <sstream>
#include <memory>
#include <vector>

#include "ast.h"

class GeneratorNodeVisitor : public NodeVisitor {
public:
    std::string generate(std::shared_ptr<Node> tree);

private:
    void add_identifier(std::string_view identifier);
    void check_is_declared(std::string_view identifier);
    static std::string create_label();

    void write_padded_identifier(std::string_view identifier);
    void write_instruction(std::string instruction);
    void write_hex(int number);
    void write_number(int number);
    void write_identifier(std::string_view identifier);
    void write_comment(const std::string& comment);
    void write_end_line();
    void write_line(std::string_view identifier, std::string instruction, std::string operand, std::string comment = "");

    void push();
    void pop();

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

    std::vector<std::string_view> m_identifiers{};
    size_t m_max_lpad{0};
    size_t m_max_rpad{0};
    size_t m_last_operand_size{0};
    size_t m_label_count{0};
    std::stringstream m_output{};
    bool m_first_pass{true};
    std::string m_next_label;
};

#endif //MIMA_COMPILER_GENERATOR_H
