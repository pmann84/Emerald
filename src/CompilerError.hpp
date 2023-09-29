#pragma once

#include <string>
#include <vector>
#include <iostream>

struct Error
{
    TokenInfo Info;
    std::string Message;

    friend std::ostream& operator<<(std::ostream& os, const Error& error);
};

inline std::ostream& operator<<(std::ostream& os, const Error& error)
{
    os << "Error: Ln " << error.Info.Line << ":" << error.Info.Pos << ": " << error.Message;
    return os;
}

class ErrorHandler
{
public:
    using container = std::vector<Error>;
    using iterator = container::iterator;
    using const_iterator = container::const_iterator;

    ErrorHandler() = default;

    [[nodiscard]] bool hasErrored() const { return !m_errors.empty(); }

    inline ErrorHandler& operator<<(Error e)
    {
        m_errors.push_back(std::move(e));
        return *this;
    }

    iterator begin() { return m_errors.begin(); }
    iterator end() { return m_errors.end(); }
    [[nodiscard]] const_iterator begin() const { return m_errors.begin(); }
    [[nodiscard]] const_iterator end() const { return m_errors.end(); }

private:
    std::vector<Error> m_errors;
};