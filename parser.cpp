#include "parser.h"

#include <iostream>

#include "debug.h"

void ParserNodeVisitor::assert_token(bool assertion) {
    if (assertion) {
        return;
    }

    std::cerr << "Invalid token '" << m_tokens->peek().string << "' found" << std::endl;
    exit(-1);
}

Token ParserNodeVisitor::next() {
    Token token = m_tokens->peek();

    if (token.type == Invalid) {
        std::cerr << "Unexpected EOF found" << std::endl;
        exit(-1);
    }

    m_tokens->next();

    return token;
}

Token ParserNodeVisitor::next_non_space() {
    Token token = next();

    while (token.type == Space) {
        token = next();
    }

    return token;
}

Token ParserNodeVisitor::peek_next_non_space()
{
    int index = 0;
    Token token = m_tokens->peek(index++);

    while (token.type == Space) {
        if (token.type == Invalid) {
            return token;
        }

        token = m_tokens->peek(index++);
    }

    return token;
}

std::shared_ptr<Statement> ParserNodeVisitor::ast_determine_statement()
{
    if (!m_tokens->hasNext()) {
        return std::make_shared<EpsilonStatement>();
    }

    auto token = m_tokens->peek();

    if (token.type == Space && m_tokens->hasNext()) {
        m_tokens->next();
        token = m_tokens->peek();
    }

    if (token.string == "var") {
        return std::make_shared<VarStatement>();
    } else if (token.string == "[") {
        return std::make_shared<OriginStatement>();
    } else if (token.string == "if") {
        return std::make_shared<ConditionalStatement>();
    } else if (token.string == "while") {
        return std::make_shared<WhileStatement>();
    } else if (token.type == Identifier) {
        return std::make_shared<AssignmentStatement>();
    }

    return std::make_shared<EpsilonStatement>();
}

void ParserNodeVisitor::visit_var_statement(VarStatement *node, int visit_count) {
    Token token = next_non_space();
    assert_token(token.string == "var");

    token = next();
    assert_token(token.type == Space);

    token = next_non_space();
    assert_token(token.type == Identifier);
    node->set_identifier(token.string);

    token = next_non_space();

    if (token.string == "=") {
        node->set_has_initial_value(true);

        token = next_non_space();
        assert_token(token.type == Value);
        node->set_number(token.number);

        token = next_non_space();
    }

    assert_token(token.string == ";");

    node->set_next(ast_determine_statement());
}

void ParserNodeVisitor::visit_assignment_statement(AssignmentStatement *node, int visit_count) {
    if (visit_count == 0) {
        Token token = next_non_space();
        assert_token(token.type == Identifier);
        node->set_identifier(token.string);

        token = next_non_space();
        assert_token(token.string == "=");

        node->set_expression(std::make_shared<NumberExpression1>());
    } else {
        Token token = next_non_space();
        assert_token(token.string == ";");

        node->set_next(ast_determine_statement());
    }
}

void ParserNodeVisitor::visit_origin_statement(OriginStatement *node, int visit_count) {
    Token token = next_non_space();
    assert_token(token.string == "[");

    token = next_non_space();
    assert_token(token.string == "org");

    token = next();
    assert_token(token.type == Space);

    token = next_non_space();
    assert_token(token.type == Value);
    node->set_number(token.number);

    token = next_non_space();
    assert_token(token.string == "]");

    node->set_next(ast_determine_statement());
}

void ParserNodeVisitor::visit_conditional_statement(ConditionalStatement *node, int visit_count) {
    if (visit_count == 0) {
        Token token = next_non_space();
        assert_token(token.string == "if");

        token = next_non_space();
        assert_token(token.string == "(");

        node->set_bool_expression(std::make_shared<BooleanExpression1>());
    } else if (visit_count == 1) {
        Token token = next_non_space();
        assert_token(token.string == ")");

        token = next_non_space();
        assert_token(token.string == "{");

        node->set_inner(ast_determine_statement());
    } else if (visit_count == 2) {
        Token token = next_non_space();
        assert_token(token.string == "}");

        token = peek_next_non_space();

        if (token.string == "else") {
            next_non_space();

            token = peek_next_non_space();

            if (token.string == "if") {
                node->set_next(std::make_shared<ConditionalStatement>());
                return;
            }

            next_non_space();
            assert_token(token.string == "{");

            node->set_else(ast_determine_statement());
            return;
        }

        node->set_next(ast_determine_statement());
    } else if (node->get_else()) {
        Token token = next_non_space();
        assert_token(token.string == "}");

        node->set_next(ast_determine_statement());
    }
}

void ParserNodeVisitor::visit_while_statement(WhileStatement *node, int visit_count) {
    if (visit_count == 0) {
        Token token = next_non_space();
        assert_token(token.string == "while");

        token = next_non_space();
        assert_token(token.string == "(");

        node->set_bool_expression(std::make_shared<BooleanExpression1>());
    } else if (visit_count == 1) {
        Token token = next_non_space();
        assert_token(token.string == ")");

        token = next_non_space();
        assert_token(token.string == "{");

        node->set_inner(ast_determine_statement());
    } else if (visit_count == 2) {
        Token token = next_non_space();
        assert_token(token.string == "}");

        node->set_next(ast_determine_statement());
    }
}

void ParserNodeVisitor::visit_epsilon_statement(EpsilonStatement *node, int visit_count) {
    node->set_next(nullptr);
}

void ParserNodeVisitor::visit_number_expression_1(NumberExpression1 *node, int visit_count) {
    if (visit_count == 0) {
        node->set_expression(std::make_shared<NumberExpression2>());
    } else if (visit_count == 1) {
        Token token = peek_next_non_space();

        if (token.string == "&") {
            next_non_space();
            node->set_bitwise_and(std::make_shared<NumberExpression1>());
        }
    }
}

void ParserNodeVisitor::visit_number_expression_2(NumberExpression2 *node, int visit_count) {
    if (visit_count == 0) {
        node->set_expression(std::make_shared<NumberExpression3>());
    } else if (visit_count == 1) {
        Token token = peek_next_non_space();

        if (token.string == ">>") {
            next_non_space();
            node->set_bitshift(std::make_shared<NumberExpression2>());
            node->set_left(false);
        }
    }
}

void ParserNodeVisitor::visit_number_expression_3(NumberExpression3 *node, int visit_count) {
    if (visit_count == 0) {
        node->set_expression(std::make_shared<NumberExpression4>());
    } else if (visit_count == 1) {
        Token token = peek_next_non_space();

        if (token.string == "+") {
            next_non_space();
            node->set_addition(std::make_shared<NumberExpression3>());
        }
    }
}

void ParserNodeVisitor::visit_number_expression_4(NumberExpression4 *node, int visit_count) {
    if (visit_count == 0) {
        Token token = peek_next_non_space();

        if (token.string == "-") {
            next_non_space();
            node->set_negated(true);
        }

        node->set_expression(std::make_shared<NumberExpression5>());
    }
}

void ParserNodeVisitor::visit_number_expression_5(NumberExpression5 *node, int visit_count) {
    if (visit_count == 0) {
        Token token = peek_next_non_space();

        if (token.string == "(") {
            next_non_space();
            node->set_expression(std::make_shared<NumberExpression1>());
            node->set_nested(true);
        } else if (token.type == Identifier) {
            node->set_expression(std::make_shared<VariableExpression>());
        } else if (token.type == Value) {
            node->set_expression(std::make_shared<ValueExpression>());
        } else {
            std::cerr << "Invalid expression '" << token.string << "'" << std::endl;
            exit(-1);
        }
    } else if (visit_count == 1 && node->is_nested()) {
        Token token = next_non_space();
        assert_token(token.string == ")");
    }
}

void ParserNodeVisitor::visit_boolean_expression_1(BooleanExpression1 *node, int visit_count) {
    if (visit_count == 0) {
        node->set_expression(std::make_shared<BooleanExpression2>());
    } else if (visit_count == 1) {
        Token token = peek_next_non_space();

        if (token.string == "||") {
            next_non_space();
            node->set_logical_or(std::make_shared<BooleanExpression1>());
        }
    }
}

void ParserNodeVisitor::visit_boolean_expression_2(BooleanExpression2 *node, int visit_count) {
    if (visit_count == 0) {
        node->set_expression(std::make_shared<BooleanExpression3>());
    } else if (visit_count == 1) {
        Token token = peek_next_non_space();

        if (token.string == "&&") {
            next_non_space();
            node->set_logical_and(std::make_shared<BooleanExpression2>());
        }
    }
}

void ParserNodeVisitor::visit_boolean_expression_3(BooleanExpression3 *node, int visit_count) {
    if (visit_count == 0) {
        Token token = peek_next_non_space();

        if (token.string == "!") {
            next_non_space();
            node->set_negated(true);
        }

        node->set_expression(std::make_shared<BooleanExpression4>());
    }
}

void ParserNodeVisitor::visit_boolean_expression_4(BooleanExpression4 *node, int visit_count) {
    if (visit_count == 0) {
        Token token = peek_next_non_space();

        if (token.string == "(") {
            next_non_space();
            node->set_expression(std::make_shared<BooleanExpression1>());
            node->set_nested(true);
        } else if (token.string == "true" | token.string == "false") {
            node->set_expression(std::make_shared<BooleanValueExpression>());
        } else {
            node->set_is_comparison(true);
            node->set_left(std::make_shared<NumberExpression1>());
        }
    } else if (visit_count == 1 && node->is_nested()) {
        Token token = next_non_space();
        assert_token(token.string == ")");
    } else if (visit_count == 1 && node->is_comparison()) {
        Token token = next_non_space();

        if (token.string == "==") {
            node->set_comparison(Equals);
        } else if (token.string == "!=") {
            node->set_comparison(NotEquals);
        } else if (token.string == "<") {
            node->set_comparison(LessThan);
        } else if (token.string == ">") {
            node->set_comparison(GreaterThan);
        } else if (token.string == "<=") {
            node->set_comparison(LessThanOrEqual);
        } else if (token.string == ">=") {
            node->set_comparison(GreaterThanOrEqual);
        } else {
            std::cerr << "Invalid operator '" << token.string << "'" << std::endl;
            exit(-1);
        }

        node->set_right(std::make_shared<NumberExpression1>());
    }
}

void ParserNodeVisitor::visit_variable_expression(VariableExpression *node, int visit_count) {
    Token token = next_non_space();
    assert_token(token.type == Identifier);
    node->set_identifier(token.string);
}

void ParserNodeVisitor::visit_value_expression(ValueExpression *node, int visit_count) {
    Token token = next_non_space();
    assert_token(token.type == Value);
    node->set_number(token.number);
}

void ParserNodeVisitor::visit_boolean_value_expression(BooleanValueExpression *node, int visit_count) {
    Token token = next_non_space();
    assert_token(token.string == "true" | token.string == "false");
    node->set_value(token.string == "true");
}

std::shared_ptr<Node> ParserNodeVisitor::parse() {
    auto root = ast_determine_statement();
    root->visit(*this);

    PrinterNodeVisitor visitor;
    std::cout << "AST: " << std::endl;
    visitor.print_tree(root.get());
    std::cout << std::endl;

    return root;
}