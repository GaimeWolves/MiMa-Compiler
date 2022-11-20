#ifndef MIMA_COMPILER_LEXER_H
#define MIMA_COMPILER_LEXER_H

#include <vector>
#include <string>
#include <string_view>

enum TokenType {
    Invalid,
    Keyword,
    Identifier,
    Value,
    SpecialSymbol,
    Space,
};

struct Token {
    TokenType type;
    std::string_view string;
    int number;
};

class Tokenization {
public:
    explicit Tokenization(std::string file_content);

    Token peek(int next = 0) const;
    void next();
    bool hasNext() const;

private:
    std::vector<Token> m_tokens;
    size_t m_index;
    std::string m_file_content;
};


#endif //MIMA_COMPILER_LEXER_H
