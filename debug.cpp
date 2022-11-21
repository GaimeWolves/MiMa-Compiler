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
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Assign " << node->get_identifier() << " = " << std::endl;
    } else if (visit_count == 1) {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_origin_statement(OriginStatement *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Origin " << node->get_number() << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_while_statement(WhileStatement *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "While" << std::endl;
    } else if (visit_count == 2) {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_conditional_statement(ConditionalStatement *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Conditional" << std::endl;
    } else if (visit_count == 3) {
        m_depth--;
    }
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
        if (node->get_bitwise_and()) {
            std::cout << std::setw(m_depth) << " " << "&" << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_2(NumberExpression2 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (2)" << std::endl;
    } else if (visit_count == 1) {
        if (node->get_bitshift()) {
            std::cout << std::setw(m_depth) << " " << (node->is_left() ? "<<" : ">>") << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_3(NumberExpression3 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (3)" << std::endl;
    } else if (visit_count == 1) {
        if (node->get_addition()) {
            std::cout << std::setw(m_depth) << " " << "+" << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_4(NumberExpression4 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (4)" << (node->is_negated() ? " negated" : "") << std::endl;
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_number_expression_5(NumberExpression5 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Number Expression (5)" << std::endl;
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_boolean_expression_1(BooleanExpression1 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Boolean Expression (1)" << std::endl;
    } else if (visit_count == 1) {
        if (node->get_logical_or()) {
            std::cout << std::setw(m_depth) << " " << "||" << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_boolean_expression_2(BooleanExpression2 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Boolean Expression (2)" << std::endl;
    } else if (visit_count == 1) {
        if (node->get_logical_and()) {
            std::cout << std::setw(m_depth) << " " << "&&" << std::endl;
        }
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_boolean_expression_3(BooleanExpression3 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Boolean Expression (3)" << (node->is_negated() ? " negated" : "") << std::endl;
    } else {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_boolean_expression_4(BooleanExpression4 *node, int visit_count) {
    if (visit_count == 0) {
        m_depth++;
        std::cout << std::setw(m_depth) << " " << "Boolean Expression (4)" << std::endl;
    } else if (visit_count == 1 && node->is_comparison()) {
        std::cout << std::setw(m_depth) << " " << (node->get_comparison() == Equals ? "==" : "<op>") << std::endl;
    } else if (visit_count == 2) {
        m_depth--;
    }
}

void PrinterNodeVisitor::visit_variable_expression(VariableExpression *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Var " << node->get_identifier() << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_value_expression(ValueExpression *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Number " << node->get_number() << std::endl;
    m_depth--;
}

void PrinterNodeVisitor::visit_boolean_value_expression(BooleanValueExpression *node, int visit_count) {
    m_depth++;
    std::cout << std::setw(m_depth) << " " << "Boolean " << (node->get_value() ? "true" : "false")  << std::endl;
    m_depth--;
}