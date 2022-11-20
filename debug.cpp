#include "debug.h"

#include <iostream>
#include <iomanip>

void PrinterNodeVisitor::visit_var_statement(VarStatement *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Var " << node->get_identifier();

    if (node->has_initial_value()) {
        std::cout << " = " << node->get_number() << std::endl;
    } else {
        std::cout << std::endl;
    }
    m_depth--;
}

void PrinterNodeVisitor::visit_assignment_statement(AssignmentStatement *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Assign " << node->get_identifier() << " = " << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_origin_statement(OriginStatement *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Origin " << node->get_number() << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_epsilon_statement(EpsilonStatement *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Epsilon" << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_number_expression_1(NumberExpression1 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (1)" << std::endl;
    } else if (visit_count == 1) {
        if (node->get_addition()) {
            std::cout << std::setw(m_depth) << " " << "+" << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_2(NumberExpression2 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (2)" << (node->is_negated() ? " negated" : "") << std::endl;
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_3(NumberExpression3 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (3)" << std::endl;
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_variable(Variable *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Var " << node->get_identifier() << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_number(Number *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Number " << node->get_number() << std::endl;
    m_depth--;
}