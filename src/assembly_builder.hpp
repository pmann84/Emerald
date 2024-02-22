#pragma once

#include <string>
#include <sstream>

class assembly_builder
{
public:
    assembly_builder() = default;

    void push(const std::string& reg) {
        m_os << "\tpush " << reg;
        end_line();
    }
    void pop(const std::string& reg) {
        m_os << "\tpop " << reg;
        end_line();
    }
    void move(const std::string& dest, const std::string& src, const std::string& comment = "") {
        m_os << "\tmov " << dest << ", " << src;
        write_comment_and_end_line(comment);
    }
    void write_label(const std::string& label, const std::string& comment = "") {
        m_os << label << ": ";
        write_comment_and_end_line(comment);
    }
    void jump_to_label(const std::string& label, const std::string& comment = "") {
        m_os << "\tjmp " << label;
        write_comment_and_end_line(comment);
    }
    void compare_and_jump(const std::string& cmp1, const std::string& cmp2, const std::string& label, const std::string& cmpComment = "") {
        m_os << "\tcmp " << cmp1 << ", " << cmp2;
        write_comment_and_end_line(cmpComment);
        m_os << "\tje " << label;
        end_line();
    }
    template<typename RhsT>
    void add(const std::string& lhs, RhsT rhs, const std::string& comment = "") {
        m_os << "\tadd " << lhs << ", " << rhs;
        write_comment_and_end_line(comment);
    }
    void sub(const std::string& lhs, const std::string& rhs, const std::string& comment = "") {
        m_os << "\tsub " << lhs << ", " << rhs;
        write_comment_and_end_line(comment);
    }
    void mul(const std::string& reg, const std::string& comment = "") {
        m_os << "\tmul " << reg;
        write_comment_and_end_line(comment);
    }
    void div(const std::string& reg, const std::string& comment = "") {
        m_os << "\tdiv " << reg;
        write_comment_and_end_line(comment);
    }
    void syscall(const std::string& comment = "") {
        m_os << "\tsyscall";
        write_comment_and_end_line(comment);
    }
    void write_comment_and_end_line(const std::string& comment = "") {
        write_comment(comment);
        end_line();
    }
    void empty_program() {
#ifdef __WIN64
        move("rax", "0");
        push("rax");
        m_os << "\tcall ExitProcess\n";
#endif
#ifdef __linux__
        move("rax", "60");
        move("rdi", "0");
        syscall();
#endif
    }
    void write_header() {
        m_os << "global start\n\n";
#ifdef __WIN64
        m_os << "section .text\n\n";
        m_os << "extern ExitProcess\n\n";
#endif
        m_os << "start:\n";
    }

    std::string str() {
        return m_os.str();
    }

private:
    void write_comment(const std::string& comment = "") {
        if (!comment.empty()) {
            m_os << " ; " << comment;
        }
    }
    void end_line() {
        m_os << "\n";
    }
private:
    std::stringstream m_os;
};
