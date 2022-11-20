#include "generator.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "ast.h"

std::string GeneratorNodeVisitor::generate(std::shared_ptr<Node> tree) {
    tree->visit(*this);
    m_first_pass = false;

    std::cout << "Declared Identifiers:" << std::endl;
    for (auto identifier : m_identifiers) {
        std::cout << identifier << std::endl;
    }
    std::cout << std::endl;

    for (auto identifier : m_identifiers) {
        if (identifier.size() > m_max_size) {
            m_max_size = identifier.size();
        }
    }

    m_output << "TODO: Add temporary variables/constants __aux __one __m_one somewhere:" << std::endl;
    m_output << "__aux DS" << std::endl;
    m_output << "__one DS 1" << std::endl;

    m_output << std::endl;
    m_output << "TODO: Set stack pointer __sp somewhere:" << std::endl;
    m_output << "__sp DS 0x10000" << std::endl;

    tree->visit(*this);
    return m_output.str();
}

void GeneratorNodeVisitor::add_identifier(std::string_view identifier) {
    if (std::find(m_identifiers.begin(), m_identifiers.end(), identifier) != m_identifiers.end()) {
        std::cerr << "Multiple declarations of '" << identifier << "' found" << std::endl;
        exit(-1);
    }

    m_identifiers.push_back(identifier);
}

void GeneratorNodeVisitor::check_is_declared(std::string_view identifier) {
    if (std::find(m_identifiers.begin(), m_identifiers.end(), identifier) == m_identifiers.end()) {
        std::cerr << "No declaration of '" << identifier << "' found" << std::endl;
        exit(-1);
    }
}

void GeneratorNodeVisitor::write_padded_identifier(std::string_view identifier) {
    int padding = m_max_size - identifier.size();

    if (padding) {
        m_output << std::setw(padding) << " ";
    }

    m_output << identifier;
}

void GeneratorNodeVisitor::write_instruction(std::string instruction) {
    m_output << " " << std::setw(4) << instruction << " ";
}

void GeneratorNodeVisitor::write_hex(int number) {
    m_output << "0x" << std::hex << number;
}

void GeneratorNodeVisitor::write_number(int number) {
    m_output << number;
}

void GeneratorNodeVisitor::write_identifier(std::string_view identifier) {
    m_output << identifier;
}

void GeneratorNodeVisitor::write_end_line() {
    m_output << std::endl;
}

void GeneratorNodeVisitor::push() {
    write_padded_identifier("");
    write_instruction("STIV");
    write_identifier("__sp");
    write_end_line();

    write_padded_identifier("");
    write_instruction("LDV");
    write_identifier("__sp");
    write_end_line();

    write_padded_identifier("");
    write_instruction("ADD");
    write_identifier("__m_one");
    write_end_line();

    write_padded_identifier("");
    write_instruction("STV");
    write_identifier("__sp");
    write_end_line();
}

void GeneratorNodeVisitor::pop() {
    write_padded_identifier("");
    write_instruction("LDV");
    write_identifier("__sp");
    write_end_line();

    write_padded_identifier("");
    write_instruction("ADD");
    write_identifier("__one");
    write_end_line();

    write_padded_identifier("");
    write_instruction("STV");
    write_identifier("__sp");
    write_end_line();

    write_padded_identifier("");
    write_instruction("LDIV");
    write_identifier("__sp");
    write_end_line();
}

void GeneratorNodeVisitor::visit_var_statement(VarStatement *node, int visit_count) {
    if (m_first_pass) {
        add_identifier(node->get_identifier());
        return;
    }

    write_padded_identifier(node->get_identifier());
    write_instruction("DS");

    if (node->has_initial_value()) {
        write_number(node->get_number());
    }

    write_end_line();
}

void GeneratorNodeVisitor::visit_assignment_statement(AssignmentStatement *node, int visit_count) {
    if (m_first_pass) {
        check_is_declared(node->get_identifier());
        return;
    }

    if (visit_count == 1) {
        write_padded_identifier("");
        write_instruction("STV");
        write_identifier(node->get_identifier());
        write_end_line();
    }
}

void GeneratorNodeVisitor::visit_origin_statement(OriginStatement *node, int visit_count) {
    if (m_first_pass) { return; }

    write_end_line();
    write_padded_identifier("*");
    write_instruction("=");
    write_hex(node->get_number());
    write_end_line();
}

void GeneratorNodeVisitor::visit_epsilon_statement(EpsilonStatement *node, int visit_count) {
    if (m_first_pass) { return; }

    write_padded_identifier("");
    write_instruction("HALT");
    write_end_line();
}

void GeneratorNodeVisitor::visit_number_expression_1(NumberExpression1 *node, int visit_count) {
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->get_addition()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_addition()) {
            write_padded_identifier("");
            write_instruction("STV");
            write_identifier("__aux");
            write_end_line();

            pop();

            write_padded_identifier("");
            write_instruction("ADD");
            write_identifier("__aux");
            write_end_line();
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_2(NumberExpression2 *node, int visit_count) {
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->is_negated()) {
            write_padded_identifier("");
            write_instruction("NOT");
            write_end_line();

            write_padded_identifier("");
            write_instruction("ADD");
            write_identifier("__one");
            write_end_line();
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_3(NumberExpression3 *node, int visit_count) {
    if (m_first_pass) { return; }
}

void GeneratorNodeVisitor::visit_variable(Variable *node, int visit_count) {
    if (m_first_pass) {
        check_is_declared(node->get_identifier());
        return;
    }

    write_padded_identifier("");
    write_instruction("LDV");
    write_identifier(node->get_identifier());
    write_end_line();
}

void GeneratorNodeVisitor::visit_number(Number *node, int visit_count) {
    if (m_first_pass) { return; }

    write_padded_identifier("");
    write_instruction("LDC");
    write_number(node->get_number());
    write_end_line();
}