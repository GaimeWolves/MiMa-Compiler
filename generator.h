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

    void write_padded_identifier(std::string_view identifier);
    void write_instruction(std::string instruction);
    void write_hex(int number);
    void write_number(int number);
    void write_identifier(std::string_view identifier);
    void write_end_line();

    void push();
    void pop();

    void visit_var_statement(VarStatement *node, int visit_count) override;
    void visit_assignment_statement(AssignmentStatement *node, int visit_count) override;
    void visit_origin_statement(OriginStatement *node, int visit_count) override;
    void visit_epsilon_statement(EpsilonStatement *node, int visit_count) override;
    void visit_number_expression_1(NumberExpression1 *node, int visit_count) override;
    void visit_number_expression_2(NumberExpression2 *node, int visit_count) override;
    void visit_number_expression_3(NumberExpression3 *node, int visit_count) override;
    void visit_variable(Variable *node, int visit_count) override;
    void visit_number(Number *node, int visit_count) override;

    std::vector<std::string_view> m_identifiers{};
    size_t m_max_size{0};
    std::stringstream m_output{};
    bool m_first_pass{true};
};

#endif //MIMA_COMPILER_GENERATOR_H
