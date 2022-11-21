#include "generator.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>

#include "ast.h"

static std::string s_aux = ".aux";
static std::string s_one = ".one";
static std::string s_m_one = ".m_one";
static std::string s_mask = ".mask";
static std::string s_sp = ".sp";
static std::string s_label_prefix = ".L";

std::string GeneratorNodeVisitor::generate(std::shared_ptr<Node> tree) {
    tree->visit(*this);
    m_first_pass = false;
    m_next_label = "";

    add_identifier(s_aux);
    add_identifier(s_one);
    add_identifier(s_m_one);
    add_identifier(s_mask);
    add_identifier(s_sp);

    std::cout << "Declared Identifiers:" << std::endl;
    for (auto identifier : m_identifiers) {
        std::cout << identifier << std::endl;
    }
    std::cout << std::endl;

    for (auto identifier : m_identifiers) {
        if (identifier.size() > m_max_lpad) {
            m_max_lpad = identifier.size();
        }
    }

    size_t max_label_size = 3 + m_label_count / 10;
    if (max_label_size > m_max_lpad) {
        m_max_lpad = max_label_size;
    }

    m_max_rpad = m_max_lpad < 8 ? 8 : m_max_lpad;

    write_line(s_aux, "DS", "", "second general purpose register");
    write_line(s_one, "DS", "1", "constant one");
    write_line(s_m_one, "DS", "-1", "constant minus one");
    write_line(s_mask, "DS", "0xFFFFFE", "bitmask for use in >>");
    write_line(s_sp, "DS", "0x10000", "set to high memory");

    tree->visit(*this);

    // TODO: Find syntax to determine HALT
    write_line("", "HALT", "");

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

std::string GeneratorNodeVisitor::create_label() {
    static size_t current = 0;

    std::string label;
    label += s_label_prefix;
    label += std::to_string(current++);

    return label;
}

void GeneratorNodeVisitor::write_padded_identifier(std::string_view identifier) {
    if (!m_next_label.empty()) {
        if (!identifier.empty()) {
            std::cout << "TODO: Multiple labels on line" << std::endl;
        }

        identifier = m_next_label;
    }

    int padding = (int)(m_max_lpad - identifier.size());

    if (padding) {
        m_output << std::setw(padding) << " ";
    }

    m_output << identifier;

    if (!m_next_label.empty()) {
        m_next_label = "";
    }
}

void GeneratorNodeVisitor::write_instruction(std::string instruction) {
    m_output << " " << std::setw(4) << instruction << " ";
}

void GeneratorNodeVisitor::write_hex(int number) {
    if (number < 0) {
        number *= -1;
        number |= (1 << 23);
    }

    std::stringstream hex;
    hex << "0x" << std::hex << number << std::dec;

    m_output << hex.str();
    m_last_operand_size = hex.str().size();
}

void GeneratorNodeVisitor::write_number(int number) {
    std::stringstream num;
    num << number;
    m_output << num.str();
    m_last_operand_size = num.str().size();
}

void GeneratorNodeVisitor::write_identifier(std::string_view identifier) {
    m_output << identifier;
    m_last_operand_size = identifier.size();
}

void GeneratorNodeVisitor::write_comment(const std::string& comment) {
    if (!comment.empty()) {
        int padding = (int) (m_max_rpad - m_last_operand_size);

        if (padding) {
            m_output << std::setw(padding) << " ";
        }

        m_output << " ;" << comment;
    }
}

void GeneratorNodeVisitor::write_end_line() {
    m_output << std::endl;
}

void GeneratorNodeVisitor::write_line(std::string_view identifier, std::string instruction, std::string operand, std::string comment)
{
    write_padded_identifier(identifier);
    write_instruction(std::move(instruction));
    write_identifier(operand);
    write_comment(comment);
    write_end_line();
}

void GeneratorNodeVisitor::push() {
    write_line("", "STIV", s_sp);
    write_line("", "LDV", s_sp);
    write_line("", "ADD", s_m_one);
    write_line("", "STV", s_sp, "push AKKU -> <sp>; sp--");
}

void GeneratorNodeVisitor::pop() {
    write_line("", "LDV", s_sp);
    write_line("", "ADD", s_one);
    write_line("", "STV", s_sp);
    write_line("", "LDIV", s_sp, "pop <sp + 1> -> AKKU; sp++");
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
        std::string identifier(node->get_identifier());
        std::string comment = identifier;
        comment += " = <expression>";
        write_line("", "STV", identifier, comment);
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

void GeneratorNodeVisitor::visit_conditional_statement(ConditionalStatement *node, int visit_count) {
    static std::string labelIf;
    static std::string labelElse;
    static std::string labelFinally;

    if (m_first_pass) {
        if (visit_count == 0) {
            m_label_count += node->get_else() ? 3 : 2;
        }
        return;
    }

    if (visit_count == 1) {
        labelIf = create_label();
        labelElse = create_label();

        if (node->get_else()) {
            labelFinally = create_label();
        }

        write_line("", "NOT", "", "boolean (0, 1) was AKKU");
        write_line("", "ADD", s_one);
        write_line("", "JMN", labelIf, "jump to if block");
        write_line("", "JMP", labelElse, "jump to else block");
        m_next_label = labelIf;
    } else if (visit_count == 2) {
        if (node->get_else()) {
            write_line("", "JMP", labelFinally, "jump to statement after if");
        }
        m_next_label = labelElse;
    } else if (visit_count == 3) {
        m_next_label = labelFinally;
    }
}

void GeneratorNodeVisitor::visit_while_statement(WhileStatement *node, int visit_count) {
    static std::string labelTop;
    static std::string labelFinally;

    if (m_first_pass) {
        if (visit_count == 0) {
            m_label_count += 2;
        }
        return;
    }

    if (visit_count == 0) {
        labelTop = create_label();
        m_next_label = labelTop;
    } else if (visit_count == 1) {
        labelFinally = create_label();
        write_line("", "ADD", s_m_one, "boolean (0, 1) was AKKU");
        write_line("", "JMN", labelFinally, "jump to statement after while");
    } else if (visit_count == 2) {
        write_line("", "JMP", labelTop, "jump to top of while");
        m_next_label = labelFinally;
    }
}

void GeneratorNodeVisitor::visit_epsilon_statement(EpsilonStatement *node, int visit_count) { }

void GeneratorNodeVisitor::visit_number_expression_1(NumberExpression1 *node, int visit_count) {
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->get_bitwise_and()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_bitwise_and()) {
            write_line("", "STV", s_aux);
            pop();
            write_line("", "AND", s_aux, "calculate bitwise AND");
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_2(NumberExpression2 *node, int visit_count) {
    if (m_first_pass) {
        if (visit_count == 0) {
            m_label_count += 2;
        }
        return;
    }

    if (visit_count == 1) {
        if (node->get_bitshift()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_bitshift()) {
            std::string labelRepeat = create_label();
            std::string labelFinally = create_label();

            if (node->is_left()) {
                std::cerr << "<< not supported currently" << std::endl;
                exit(-1);
            }

            write_line("", "JMN", labelFinally);
            write_line("", "STV", s_aux, "calculate right shift");
            write_line(labelRepeat, "LDV", s_aux);
            write_line("", "ADD", s_m_one);
            write_line("", "JMN", labelFinally, "count aux to zero");
            write_line("", "STV", s_aux);
            pop();
            write_line("", "AND", s_mask);
            write_line("", "RAR", "", "shift one bit out");
            push();
            m_next_label = labelFinally;
            pop();
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_3(NumberExpression3 *node, int visit_count) {
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->get_addition()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_addition()) {
            write_line("", "STV", s_aux);
            pop();
            write_line("", "ADD", s_aux, "calculate addition");
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_4(NumberExpression4 *node, int visit_count) {
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->is_negated()) {
            write_line("", "NOT", "");
            write_line("", "ADD", s_one, "calculate negation");
        }
    }
}

void GeneratorNodeVisitor::visit_number_expression_5(NumberExpression5 *node, int visit_count) {
    if (m_first_pass) { return; }
}

void GeneratorNodeVisitor::visit_boolean_expression_1(BooleanExpression1 *node, int visit_count)
{
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->get_logical_or()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_logical_or()) {
            write_line("", "STV", s_aux);
            pop();
            write_line("", "OR", s_aux, "calculate boolean OR");
        }
    }
}

void GeneratorNodeVisitor::visit_boolean_expression_2(BooleanExpression2 *node, int visit_count)
{
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->get_logical_and()) {
            push();
        }
    } else if (visit_count == 2) {
        if (node->get_logical_and()) {
            write_line("", "STV", s_aux);
            pop();
            write_line("", "AND", s_aux, "calculate boolean AND");
        }
    }
}

void GeneratorNodeVisitor::visit_boolean_expression_3(BooleanExpression3 *node, int visit_count)
{
    if (m_first_pass) { return; }

    if (visit_count == 1) {
        if (node->is_negated()) {
            write_line("", "NOT", "");
            write_line("", "AND", s_one, "calculate boolean negation");
        }
    }
}

void GeneratorNodeVisitor::visit_boolean_expression_4(BooleanExpression4 *node, int visit_count)
{
    if (m_first_pass) {
        if (visit_count == 0) {
            if (node->is_comparison() && (node->get_comparison() != Equals && node->get_comparison() != NotEquals)) {
                m_label_count += 2;
            }
        }
        return;
    }

    if (visit_count == 1 && node->is_comparison()) {
        push();
    } else if (visit_count == 2 && node->is_comparison()) {
        write_line("", "STV", s_aux);
        pop();

        // left in akku; right in __aux

        std::string label1 = create_label();
        std::string label2 = create_label();

        Comparison comparison = node->get_comparison();

        switch (comparison) {
            case Equals: // left == __aux
                write_line("", "EQL", s_aux);
                write_line("", "NOT", "");
                write_line("", "ADD", s_one, "calculate boolean ==");
                break;
            case NotEquals: // left != __aux
                write_line("", "EQL", s_aux);
                write_line("", "ADD", s_one, "calculate boolean !=");
                break;
            case LessThan: // left - __aux < 0
            case GreaterThan: // __aux - left < 0
            case LessThanOrEqual: // __aux - left >= 0
            case GreaterThanOrEqual: // left - __aux >= 0
                write_line("", "NOT", "");
                write_line("", "ADD", s_one);
                write_line("", "ADD", s_aux, "calculate right (aux) - left (AKKU)");

                // akku = __aux - left

                if (comparison == LessThan || comparison == GreaterThanOrEqual) {
                    write_line("", "NOT", "");
                    write_line("", "ADD", s_one, "calculate left (AKKU) - right (aux)");
                    // akku = left - __aux
                }

                write_line("", "JMN", label1);
                write_line("", "LDC", (comparison == LessThan || comparison == GreaterThan) ? "0" : "1", "result is < 0");
                write_line("", "JMP", label2);
                write_line(label1, "LDC", (comparison == LessThan || comparison == GreaterThan) ? "1" : "0", "result is >= 0");
                m_next_label = label2;

                break;
        }
    }
}

void GeneratorNodeVisitor::visit_variable_expression(VariableExpression *node, int visit_count) {
    if (m_first_pass) {
        check_is_declared(node->get_identifier());
        return;
    }

    std::string identifier(node->get_identifier());
    std::string comment(node->get_identifier());
    comment += " -> Akku";
    write_line("", "LDV", identifier, comment);

}

void GeneratorNodeVisitor::visit_value_expression(ValueExpression *node, int visit_count) {
    if (m_first_pass) { return; }

    write_padded_identifier("");
    write_instruction("LDC");
    write_number(node->get_number());
    write_comment("constant -> AKKU");
    write_end_line();
}

void GeneratorNodeVisitor::visit_boolean_value_expression(BooleanValueExpression *node, int visit_count) {
    if (m_first_pass) { return; }

    write_padded_identifier("");
    write_instruction("LDC");
    write_number(node->get_value() ? 1 : 0);
    write_comment("load boolean true or false");
    write_end_line();
}