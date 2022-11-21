#ifndef MIMA_COMPILER_AST_H
#define MIMA_COMPILER_AST_H

#include <memory>
#include <utility>

#include "forward.h"

class NodeVisitor {
public:
    virtual void visit_var_statement(VarStatement *node, int visit_count) = 0;
    virtual void visit_assignment_statement(AssignmentStatement *node, int visit_count) = 0;
    virtual void visit_origin_statement(OriginStatement *node, int visit_count) = 0;
    virtual void visit_conditional_statement(ConditionalStatement *node, int visit_count) = 0;
    virtual void visit_while_statement(WhileStatement *node, int visit_count) = 0;
    virtual void visit_epsilon_statement(EpsilonStatement *node, int visit_count) = 0;
    virtual void visit_number_expression_1(NumberExpression1 *node, int visit_count) = 0;
    virtual void visit_number_expression_2(NumberExpression2 *node, int visit_count) = 0;
    virtual void visit_number_expression_3(NumberExpression3 *node, int visit_count) = 0;
    virtual void visit_number_expression_4(NumberExpression4 *node, int visit_count) = 0;
    virtual void visit_number_expression_5(NumberExpression5 *node, int visit_count) = 0;
    virtual void visit_boolean_expression_1(BooleanExpression1 *node, int visit_count) = 0;
    virtual void visit_boolean_expression_2(BooleanExpression2 *node, int visit_count) = 0;
    virtual void visit_boolean_expression_3(BooleanExpression3 *node, int visit_count) = 0;
    virtual void visit_boolean_expression_4(BooleanExpression4 *node, int visit_count) = 0;
    virtual void visit_variable_expression(VariableExpression *node, int visit_count) = 0;
    virtual void visit_value_expression(ValueExpression *node, int visit_count) = 0;
    virtual void visit_boolean_value_expression(BooleanValueExpression *node, int visit_count) = 0;
};

enum Comparison {
    Equals,
    NotEquals,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
};

class Node {
public:
    virtual void visit(NodeVisitor &visitor) = 0;
};

class Statement : public Node {
public:
    void set_next(std::shared_ptr<Statement> statement) { m_next = std::move(statement); }
    [[nodiscard]] std::shared_ptr<Statement> get_next() const { return m_next; }

protected:
    std::shared_ptr<Statement> m_next{nullptr};
};

class VarStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_var_statement(this, 0);
        m_next->visit(visitor);
    };

    void set_identifier(std::string_view identifier) { m_identifier = identifier; }
    [[nodiscard]] std::string_view get_identifier() const { return m_identifier; }

    void set_has_initial_value(bool has_initial_value) { m_has_initial_value = has_initial_value; }
    [[nodiscard]] bool has_initial_value() const { return m_has_initial_value; }

    void set_number(int number) { m_number = number; }
    [[nodiscard]] int get_number() const { return m_number; }

private:
    std::string_view m_identifier{};
    bool m_has_initial_value{};
    int m_number{};
};

class AssignmentStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_assignment_statement(this, 0);
        m_expression->visit(visitor);
        visitor.visit_assignment_statement(this, 1);
        m_next->visit(visitor);
    };

    void set_identifier(std::string_view identifier) { m_identifier = identifier; }
    [[nodiscard]] std::string_view get_identifier() const { return m_identifier; }

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

private:
    std::string_view m_identifier{};
    std::shared_ptr<Node> m_expression{nullptr};
};

class OriginStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_origin_statement(this, 0);
        m_next->visit(visitor);
    };

    void set_number(int number) { m_number = number; }
    [[nodiscard]] int get_number() const { return m_number; }

private:
    int m_number{};
};

class ConditionalStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_conditional_statement(this, 0);
        m_bool_expression->visit(visitor);
        visitor.visit_conditional_statement(this, 1);
        m_inner->visit(visitor);
        visitor.visit_conditional_statement(this, 2);
        if (m_else) {
            m_else->visit(visitor);
            visitor.visit_conditional_statement(this, 3);
        }
        m_next->visit(visitor);
    };

    void set_bool_expression(std::shared_ptr<Node> expression) { m_bool_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_bool_expression() const { return m_bool_expression; }

    void set_inner(std::shared_ptr<Node> expression) { m_inner = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_inner() const { return m_inner; }

    void set_else(std::shared_ptr<Node> expression) { m_else = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_else() const { return m_else; }

private:
    std::shared_ptr<Node> m_bool_expression;
    std::shared_ptr<Node> m_inner;
    std::shared_ptr<Node> m_else;
};

class WhileStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_while_statement(this, 0);
        m_bool_expression->visit(visitor);
        visitor.visit_while_statement(this, 1);
        m_inner->visit(visitor);
        visitor.visit_while_statement(this, 2);
        m_next->visit(visitor);
    };

    void set_bool_expression(std::shared_ptr<Node> expression) { m_bool_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_bool_expression() const { return m_bool_expression; }

    void set_inner(std::shared_ptr<Node> expression) { m_inner = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_inner() const { return m_inner; }

private:
    std::shared_ptr<Node> m_bool_expression;
    std::shared_ptr<Node> m_inner;
};

class EpsilonStatement : public Statement {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_epsilon_statement(this, 0);
    };
};

class NumberExpression1 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_1(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_1(this, 1);
        if (m_bitwise_and) {
            m_bitwise_and->visit(visitor);
        }
        visitor.visit_number_expression_1(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_bitwise_and(std::shared_ptr<Node> bitwise_and) { m_bitwise_and = std::move(bitwise_and); }
    [[nodiscard]] std::shared_ptr<Node> get_bitwise_and() const { return m_bitwise_and; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_bitwise_and{nullptr};
};

class NumberExpression2 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_2(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_2(this, 1);
        if (m_bitshift) {
            m_bitshift->visit(visitor);
        }
        visitor.visit_number_expression_2(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_bitshift(std::shared_ptr<Node> bitshift) { m_bitshift = std::move(bitshift); }
    [[nodiscard]] std::shared_ptr<Node> get_bitshift() const { return m_bitshift; }

    void set_left(bool left) { m_left = left; }
    [[nodiscard]] bool is_left() const { return m_left; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_bitshift{nullptr};
    bool m_left{false};
};

class NumberExpression3 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_3(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_3(this, 1);
        if (m_addition) {
            m_addition->visit(visitor);
        }
        visitor.visit_number_expression_3(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_addition(std::shared_ptr<Node> addition) { m_addition = std::move(addition); }
    [[nodiscard]] std::shared_ptr<Node> get_addition() const { return m_addition; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_addition{nullptr};
};

class NumberExpression4 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_4(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_4(this, 1);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_negated(bool negated) { m_negated = negated; }
    [[nodiscard]] bool is_negated() const { return m_negated; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    bool m_negated{};
};

class NumberExpression5 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_5(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_5(this, 1);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_nested(bool is_nested) { m_is_nested = true; }
    [[nodiscard]] bool is_nested() const { return m_is_nested; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    bool m_is_nested{false};
};

class BooleanExpression1 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_boolean_expression_1(this, 0);
        m_expression->visit(visitor);
        visitor.visit_boolean_expression_1(this, 1);
        if (m_logical_or) {
            m_logical_or->visit(visitor);
        }
        visitor.visit_boolean_expression_1(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_logical_or(std::shared_ptr<Node> other) { m_logical_or = std::move(other); }
    [[nodiscard]] std::shared_ptr<Node> get_logical_or() const { return m_logical_or; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_logical_or{nullptr};
};

class BooleanExpression2 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_boolean_expression_2(this, 0);
        m_expression->visit(visitor);
        visitor.visit_boolean_expression_2(this, 1);
        if (m_logical_and) {
            m_logical_and->visit(visitor);
        }
        visitor.visit_boolean_expression_2(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_logical_and(std::shared_ptr<Node> other) { m_logical_and = std::move(other); }
    [[nodiscard]] std::shared_ptr<Node> get_logical_and() const { return m_logical_and; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_logical_and{nullptr};
};

class BooleanExpression3 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_boolean_expression_3(this, 0);
        m_expression->visit(visitor);
        visitor.visit_boolean_expression_3(this, 1);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_negated(bool negated) { m_negated = true; }
    [[nodiscard]] bool is_negated() const { return m_negated; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    bool m_negated{};
};

class BooleanExpression4 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_boolean_expression_4(this, 0);
        if (!m_is_comparison) {
            m_expression->visit(visitor);
        } else {
            m_left->visit(visitor);
        }
        visitor.visit_boolean_expression_4(this, 1);
        if (m_is_comparison) {
            m_right->visit(visitor);
            visitor.visit_boolean_expression_4(this, 2);
        }

    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_left(std::shared_ptr<Node> expression) { m_left = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_left() const { return m_left; }

    void set_comparison(Comparison comparison) { m_comparison = comparison; }
    [[nodiscard]] Comparison get_comparison() const { return m_comparison; }

    void set_right(std::shared_ptr<Node> expression) { m_right = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_right() const { return m_right; }

    void set_nested(bool is_nested) { m_is_nested = is_nested; }
    [[nodiscard]] bool is_nested() const { return m_is_nested; }

    void set_is_comparison(bool is_comparison) { m_is_comparison = is_comparison; }
    [[nodiscard]] bool is_comparison() const { return m_is_comparison; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_left{nullptr};
    Comparison m_comparison{};
    std::shared_ptr<Node> m_right{nullptr};
    bool m_is_nested{false};
    bool m_is_comparison{false};
};

class VariableExpression : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_variable_expression(this, 0);
    };

    void set_identifier(std::string_view identifier) { m_identifier = identifier; }
    [[nodiscard]] std::string_view get_identifier() const { return m_identifier; }

private:
    std::string_view m_identifier{};
};

class ValueExpression : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_value_expression(this, 0);
    };

    void set_number(int number) { m_number = number; }
    [[nodiscard]] int get_number() const { return m_number; }

private:
    int m_number{};
};

class BooleanValueExpression : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_boolean_value_expression(this, 0);
    };

    void set_value(bool value) { m_value = value; }
    [[nodiscard]] int get_value() const { return m_value; }

private:
    bool m_value{};
};

#endif //MIMA_COMPILER_AST_H