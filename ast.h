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
    virtual void visit_epsilon_statement(EpsilonStatement *node, int visit_count) = 0;
    virtual void visit_number_expression_1(NumberExpression1 *node, int visit_count) = 0;
    virtual void visit_number_expression_2(NumberExpression2 *node, int visit_count) = 0;
    virtual void visit_number_expression_3(NumberExpression3 *node, int visit_count) = 0;
    virtual void visit_variable(Variable *node, int visit_count) = 0;
    virtual void visit_number(Number *node, int visit_count) = 0;
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
        if (m_addition) {
            m_addition->visit(visitor);
        }
        visitor.visit_number_expression_1(this, 2);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_addition(std::shared_ptr<Node> addition) { m_addition = std::move(addition); }
    [[nodiscard]] std::shared_ptr<Node> get_addition() const { return m_addition; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    std::shared_ptr<Node> m_addition{nullptr};
};

class NumberExpression2 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_2(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_2(this, 1);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_negated(bool negated) { m_negated = true; }
    [[nodiscard]] bool is_negated() const { return m_negated; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    bool m_negated{};
};

class NumberExpression3 : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number_expression_3(this, 0);
        m_expression->visit(visitor);
        visitor.visit_number_expression_3(this, 1);
    };

    void set_expression(std::shared_ptr<Node> expression) { m_expression = std::move(expression); }
    [[nodiscard]] std::shared_ptr<Node> get_expression() const { return m_expression; }

    void set_nested(bool is_nested) { m_is_nested = true; }
    [[nodiscard]] bool is_nested() const { return m_is_nested; }

private:
    std::shared_ptr<Node> m_expression{nullptr};
    bool m_is_nested{false};
};

class Variable : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_variable(this, 0);
    };

    void set_identifier(std::string_view identifier) { m_identifier = identifier; }
    [[nodiscard]] std::string_view get_identifier() const { return m_identifier; }

private:
    std::string_view m_identifier{};
};

class Number : public Node {
public:
    void visit(NodeVisitor &visitor) override {
        visitor.visit_number(this, 0);
    };

    void set_number(int number) { m_number = number; }
    [[nodiscard]] int get_number() const { return m_number; }

private:
    int m_number{};
};

#endif //MIMA_COMPILER_AST_H