#pragma once

#include <string>
#include <vector>
#include <iostream>

struct Message
{
    enum class Kind
    {
        Error,
        Warning,
        Info
    };
    Kind Kind;
    Token::Info Info;
    std::string Description;

    friend std::ostream& operator<<(std::ostream& os, const Message& error);
};

inline std::ostream& operator<<(std::ostream& os, const Message& error)
{
    os << "Error: Ln " << error.Info.Line << ":" << error.Info.Pos << ": " << error.Description;
    return os;
}

inline Message makeError(Token::Info info, std::string description) {
    return { .Kind = Message::Kind::Error, .Info = info, .Description = std::move(description) };
}

class ErrorHandler
{
public:
    using container = std::vector<Message>;
    using iterator = container::iterator;
    using const_iterator = container::const_iterator;

    ErrorHandler() = default;

    [[nodiscard]] bool hasErrored() const {
        const size_t errorCount = std::count_if(
                m_messages.begin(),
                m_messages.end(),
                [](const Message& m) {
                    return m.Kind == Message::Kind::Error;
                });
        return errorCount != 0;
    }

    inline ErrorHandler& operator<<(Message e)
    {
        m_messages.push_back(std::move(e));
        return *this;
    }

    iterator begin() { return m_messages.begin(); }
    iterator end() { return m_messages.end(); }
    [[nodiscard]] const_iterator begin() const { return m_messages.begin(); }
    [[nodiscard]] const_iterator end() const { return m_messages.end(); }

private:
    std::vector<Message> m_messages;
};