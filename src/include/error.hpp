#pragma once

#include <format>
#include <print>
#include <string>
#include <utility>

#include "source.hpp"

class Error {
public:
    std::string message;
    SourceSpan span;

    virtual ~Error()           = default;
    virtual void Print() const = 0;

    template <typename... Args>
    Error(SourceSpan span, std::format_string<Args...> format, Args&&... args)
        : message(std::format(format, std::forward<Args>(args)...)), span(span) {}

protected:
    template <typename... Args>
    void PrintDiagnostic(std::format_string<Args...> format, Args&&... args) const {
        std::print(stderr, "{}:{}: ", span.location.line, span.location.column);
        std::println(stderr, format, std::forward<Args>(args)...);
    }
};
