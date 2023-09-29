#include "Tokeniser.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "Assembler.hpp"
#include "Linker.hpp"
#include "CompilerError.hpp"

#include <argparse.h>

#include <iostream>
#include <fstream>

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

    // Initialise an error handler to accumulate
    // and errors
    ErrorHandler errorHandler;

    // Lexing
    Tokeniser tokeniser(std::move(srcStr), errorHandler);
    auto tokens = tokeniser.tokenise();

    // Parsing
    Parser parser(std::move(tokens), errorHandler);
    const auto ast = parser.parse();

    // Generation
    Generator generator(ast, errorHandler);
    const auto generatedAsm = generator.generateProgram();

    if (errorHandler.hasErrored())
    {
        for(const auto& error : errorHandler)
        {
            std::cerr << error << std::endl;
        }
    }
    else
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
    return 0;
}
