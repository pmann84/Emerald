#include <Tokeniser.hpp>

#include <gtest/gtest.h>

TEST(TokeniserTests, TestSingleReturnStatement)
{
    std::string testFilename = "test.emd";
    ErrorHandler handler;
    const std::string source = "return 60;";
    Tokeniser tokeniser(source, testFilename, handler);

    const auto tokens = tokeniser.tokenise();
    // Check number of tokens
    EXPECT_EQ(tokens.size(), 3);

    // Check token kinds
    EXPECT_EQ(tokens[0].kind(), Token::Kind::Return);
    EXPECT_EQ(tokens[1].kind(), Token::Kind::IntLit);
    EXPECT_EQ(tokens[2].kind(), Token::Kind::SemiColon);

    // Check token contents
    EXPECT_EQ(tokens[1].value(), "60");
}

TEST(TokeniserTests, TestCommentOnItsOwnLine)
{
    std::string testFilename = "test.emd";
    ErrorHandler handler;
    const std::string source = "# this is a comment";
    Tokeniser tokeniser(source, testFilename, handler);

    const auto tokens = tokeniser.tokenise();
    // Check number of tokens
    EXPECT_EQ(tokens.size(), 1);

    // Check token kinds
    EXPECT_EQ(tokens[0].kind(), Token::Kind::Hash);

    // Check token contents
    EXPECT_EQ(tokens[0].value(), "this is a comment");
}

TEST(TokeniserTests, TestCommentOnLine)
{
    std::string testFilename = "test.emd";
    ErrorHandler handler;
    const std::string source = "let x = 10; # this is a comment";
    Tokeniser tokeniser(source, testFilename, handler);

    const auto tokens = tokeniser.tokenise();
    // Check number of tokens
    EXPECT_EQ(tokens.size(), 6);

    // Check token kinds
    EXPECT_EQ(tokens[0].kind(), Token::Kind::Let);
    EXPECT_EQ(tokens[1].kind(), Token::Kind::Identifier);
    EXPECT_EQ(tokens[2].kind(), Token::Kind::Equals);
    EXPECT_EQ(tokens[3].kind(), Token::Kind::IntLit);
    EXPECT_EQ(tokens[4].kind(), Token::Kind::SemiColon);
    EXPECT_EQ(tokens[5].kind(), Token::Kind::Hash);

    // Check token contents
    EXPECT_EQ(tokens[1].value(), "x");
    EXPECT_EQ(tokens[3].value(), "10");
    EXPECT_EQ(tokens[5].value(), "this is a comment");
}