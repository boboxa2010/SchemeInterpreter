#pragma once

#include <stdexcept>

struct NotImplementedError : public std::logic_error {
    using std::logic_error::logic_error;
};

struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct NameError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};