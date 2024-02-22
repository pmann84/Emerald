#pragma once

#include <cstdlib>
#include <string>
#include <sstream>

class label_manager
{
public:
    label_manager() = default;

    std::string next() {
        return "L" + std::to_string(m_label_index++);
    }
private:
    size_t m_label_index = 0;
};
