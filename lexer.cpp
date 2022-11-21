#include "lexer.h"

#include <regex>
#include <iostream>
#include <utility>

static const std::regex s_regex_keyword(R"(^(var|org|if|else|true|false|while))");
static const std::regex s_regex_identifier(R"(^([a-zA-Z_][a-zA-Z0-9_\-]*))");
static const std::regex s_regex_number(R"(^(\d(?!x|b)\d*))");
static const std::regex s_regex_hex(R"(^(0x[0-9a-fA-F]+))");
static const std::regex s_regex_bin(R"(^(0b[01]+))");
static const std::regex s_regex_special(R"(^(==|<=|>=|!=|>>|[=\-+[\](){}<>&|;,]))");
static const std::regex s_regex_space(R"(^(\s+))");
static const std::regex s_regex_comment(R"(^(//.*\n|//.*$))");

Tokenization::Tokenization(std::string file_content)
{
    m_file_content = std::move(file_content);

    std::cout << "Input:" << std::endl;
    std::cout << m_file_content << std::endl << std::endl;

    m_index = 0;
    m_tokens = std::vector<Token>();

    std::cout << "Matches:" << std::endl;

    long position = 0;

    while (position < m_file_content.length()) {
        std::smatch match;
        TokenType type;
        int value = 0;

        if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match, s_regex_comment)) {
            std::cout << "Comment " << match.str();
            position += match.length();
            continue;
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match, s_regex_space)) {
            std::cout << "Space";
            type = Space;
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match, s_regex_keyword)) {
            std::cout << "Keyword";
            type = Keyword;
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match,s_regex_identifier)) {
            std::cout << "Identifier";
            type = Identifier;
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match,s_regex_number)) {
            std::cout << "Number";
            type = Value;
            value = std::stoi(match.str(), nullptr, 10);
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match,s_regex_hex)) {
            std::cout << "Hex";
            type = Value;
            value = std::stoi(match.str(), nullptr, 16);
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match,s_regex_bin)) {
            std::cout << "Bin";
            type = Value;
            value = std::stoi(match.str(), nullptr, 2);
        } else if (std::regex_search(m_file_content.cbegin() + position, m_file_content.cend(), match,s_regex_special)) {
            std::cout << "Special";
            type = SpecialSymbol;
        } else {
            std::cerr << "Invalid token '" << m_file_content.at(position) << "' found" << std::endl;
            exit(-1);
        }

        std::cout << " '" << (type == Space ? " " : match.str()) << "' at " << position << std::endl;

        Token token;
        token.type = type;
        token.string = std::string_view(m_file_content.cbegin() + position, m_file_content.cbegin() + position + match.length());
        token.number = value;

        m_tokens.push_back(token);

        position += match.length();
    }

    std::cout << std::endl;
}

Token Tokenization::peek(int next) const
{
    if (m_index + next >= m_tokens.size()) {
        Token token;
        token.type = Invalid;
        return token;
    }

    return m_tokens[m_index + next];
}

void Tokenization::next()
{
    if (hasNext()) {
        m_index++;
    }
}

bool Tokenization::hasNext() const
{
    return m_index != m_tokens.size() - 1;
}