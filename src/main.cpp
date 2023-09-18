#include <iostream>

#include <argparse.h>
#include <fstream>
#include <optional>

enum class TokenType
{
    return_,
    int_lit,
    expr_end, // nominally a semi-colon
};

struct Token
{
    TokenType token;
    std::optional<std::string> value;
};

struct TokeniseResult
{
    std::vector<Token> tokens;
    bool success;
    std::vector<std::string> errors;
};

TokeniseResult tokenise(const std::string& srcStr)
{
    std::vector<Token> tokens{};
    std::vector<std::string> errors{};
    std::stringstream buf;
    for (size_t i = 0; i < srcStr.length(); ++i)
    {
        const auto srcChar = srcStr.at(i);
        if (std::isalpha(srcChar))
        {
            buf << srcChar;
            i++;
            while (std::isalnum(srcStr.at(i))) {
                buf << srcStr.at(i);
                i++;
            }
            i--;

            // Check if it's a key word
            if (buf.str() == "return")
            {
                tokens.push_back({ .token = TokenType::return_ });
                buf.str("");
            }
            else
            {
                errors.emplace_back("Error compiling source file.");
                return { .tokens = {}, .success = false, .errors = errors };
            }
        }
        else if (std::isdigit(srcChar))
        {
            buf << srcChar;
            i++;
            while (std::isdigit(srcStr.at(i))) {
                buf << srcStr.at(i);
                i++;
            }
            i--;
            tokens.push_back({ .token = TokenType::int_lit, .value = buf.str() });
            buf.str("");
        }
        else if (srcChar == ';')
        {
            tokens.push_back({ .token = TokenType::expr_end });
        }
        else if (std::isspace(srcChar))
        {
            // Ignore whitespace
            continue;
        }
        else
        {
            errors.emplace_back("Error compiling source file.");
            return { .tokens = {}, .success = false, .errors = errors };
        }
    }

    return { .tokens = tokens, .success = true, .errors = errors };
}

std::string tokensToAsm(const std::vector<Token>& tokens)
{
    std::stringstream outputAsm;
    outputAsm <<"global _start\n\n_start:\n";

    for (auto i = 0; i < tokens.size(); ++i)
    {
        const auto& token = tokens.at(i);
        if (token.token == TokenType::return_)
        {
            if (i+1 < tokens.size() && tokens.at(i+1).token == TokenType::int_lit)
            {
                if (i+2 < tokens.size() && tokens.at(i+2).token == TokenType::expr_end)
                {
                    outputAsm << "\tmov rax, 60\n";
                    outputAsm << "\tmov rdi, " << tokens.at(i+1).value.value() << "\n";
                    outputAsm << "\tsyscall\n";
                }
            }
        }
    }
    return outputAsm.str();
}

int main(int argc, char** argv)
{
    // Parse the arguments
    auto parser = argparse::argument_parser("Emerald", "");
    parser.add_argument("src").help("Source file to compile.");
    parser.parse_args(argc, argv);

    // Get the source file path
    auto srcFilePath = parser.get<std::string>("src");

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

    // Lexing
    const auto tokeniseResult = tokenise(srcStr);
    if (!tokeniseResult.success) {
        std::cerr << "Failed to compile source file " << srcFilePath << std::endl;
        std::cerr << tokeniseResult.errors.size() << " errors found: " << std::endl;
        for(const auto error : tokeniseResult.errors)
        {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    // Parsing
    const auto parsedAsm = tokensToAsm(tokeniseResult.tokens);

    // Output to a file
    {
        std::fstream outputFile("./out.asm", std::ios::out);
        outputFile << parsedAsm;
    }

    std::system("nasm -felf64 out.asm");
    std::system("ld out.o -o out");

    return 0;
}
