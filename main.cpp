#include <iostream>

#include <fstream>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "generator.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
        exit(-1);
    }

    std::ifstream file_stream(argv[1]);
    std::string file_content((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());

    Tokenization tokenization = Tokenization(file_content);
    ParserNodeVisitor visitor(tokenization);
    auto tree = visitor.parse();

    GeneratorNodeVisitor generator;
    std::string output = generator.generate(tree);

    std::cout << "Compiled file:" << std::endl;
    std::cout << output << std::endl;

    std::ofstream output_stream(argv[2], std::ios::trunc);
    output_stream << output;
    output_stream.flush();
    output_stream.close();

    return 0;
}
