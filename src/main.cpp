#include "Tokeniser.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "Assembler.hpp"
#include "Linker.hpp"

#include <argparse.h>

#include <iostream>
#include <fstream>

void handleErrors(const std::vector<std::string>& errors)
{
    std::cerr << "Failed to compile Emerald source!" << std::endl;
    for (auto& error : errors)
    {
        std::cerr << "Error: " << error << std::endl;
    }
    exit(1);
}

int main(int argc, char** argv)
{
    // Parse the arguments
    auto argParser = argparse::argument_parser("Emerald", "");
    argParser.add_argument("src").help("Source file to compile.");
    argParser.add_argument({"-o", "-out"}).help("Optional output name.");
    argParser.parse_args(argc, argv);

    // Get the source file path
    auto srcFilePath = argParser.get<std::string>("src");

    // Open the file for reading
    std::fstream srcFile(srcFilePath, std::ios::in);
    if (!srcFile) {
        std::cerr << "Emerald source file " << srcFilePath << " not found." << std::endl;
        return 1;
    }

    // Read the file into a stream
    std::string srcStr;
    {
        std::stringstream srcStream;
        if (srcFile.is_open()) {
            srcStream << srcFile.rdbuf();
            srcStr = srcStream.str();
        } else {
            std::cerr << "Failed to open Emerald source file " << srcFilePath << std::endl;
            return 1;
        }
    }

    // Initialise a compile result
    Result<int> compileResult = { .result = 0, .success = true, .errors = {}};

    // Lexing
    Tokeniser tokeniser(std::move(srcStr));
    auto tokeniseResult = tokeniser.tokenise();
    if (!tokeniseResult.success) {
        compileResult.errors.insert(
            compileResult.errors.end(),
            tokeniseResult.errors.begin(),
            tokeniseResult.errors.end()
        );
    }

    // Parsing
    Parser parser(std::move(tokeniseResult.result));
    const auto ast = parser.parse();

    if (!ast.success)
    {
        compileResult.errors.insert(
                compileResult.errors.end(),
                ast.errors.begin(),
                ast.errors.end()
        );
        handleErrors(compileResult.errors);
    }

    // Generation
    Generator generator(ast.result);
    const auto generatedAsm = generator.generateProgram();

    compileResult.success = compileResult.errors.empty();

    if (compileResult.success)
    {
        // Construct output name
        std::string outName = "out";
        argParser.try_get<std::string>("out", outName);

        // Generate assembly
        Assembler assembler(outName);
        assembler.generate(generatedAsm);

        // Link
        Linker linker(outName);
        linker.link();
    }
    else
    {
        handleErrors(compileResult.errors);
    }
    return 0;
}
