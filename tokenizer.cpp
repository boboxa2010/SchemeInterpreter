#include "error.h"
#include "tokenizer.h"

namespace {
    bool IsSymbol(char c) {
        return !std::isspace(c) && c != '(' && c != ')' && c != '\'' && c != '.' &&
               c != std::istream::traits_type::eof();
    }

    bool IsValidASCIISymbol(char c) {
        return std::isalnum(c) || std::isdigit(c) || c == '<' || c == '=' || c == '>' || c == '*' ||
               c == '/' || c == '#' || c == '?' || c == '!' || c == '-' || c == '+';
    }
}  // namespace

Tokenizer::Tokenizer(std::istream *in) : stream_(in), curr_token_(SymbolToken({})) {
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

void Tokenizer::Next() {
    char current = stream_->get();

    while (std::isspace(current)) {
        current = stream_->get();
    }

    if (current == std::istream::traits_type::eof()) {
        is_end_ = true;
        return;
    }

    if (current == '\'') {
        curr_token_ = QuoteToken();
        return;
    }

    if (current == '.') {
        curr_token_ = DotToken();
        return;
    }

    if (current == '(') {
        curr_token_ = BracketToken::OPEN;
        return;
    }

    if (current == ')') {
        curr_token_ = BracketToken::CLOSE;
        return;
    }

    if (std::isdigit(current)) {
        std::string value;
        value += current;
        while (std::isdigit(stream_->peek())) {
            value += stream_->get();
        }
        curr_token_ = ConstantToken{stoi(value)};
        return;
    }

    if (IsValidASCIISymbol(current)) {
        if (current == '-' && std::isdigit(stream_->peek())) {
            std::string value = "-";
            value += stream_->get();
            while (std::isdigit(stream_->peek())) {
                value += stream_->get();
            }
            curr_token_ = ConstantToken{stoi(value)};
            return;
        }
        if (current == '+' && std::isdigit(stream_->peek())) {
            std::string value;
            value += stream_->get();
            while (std::isdigit(stream_->peek())) {
                value += stream_->get();
            }
            curr_token_ = ConstantToken{stoi(value)};
            return;
        }
        if (current == '#' && stream_->peek() == 't') {
            stream_->get();
            curr_token_ = BooleanToken{true};
            return;
        }
        if (current == '#' && stream_->peek() == 'f') {
            stream_->get();
            curr_token_ = BooleanToken{false};
            return;
        }
        std::string value;
        value += current;
        while (IsSymbol(stream_->peek())) {
            value += stream_->get();
            if (stream_->peek() == std::istream::traits_type::eof()) {
                break;
            }
        }
        curr_token_ = SymbolToken{value};
        return;
    }
    throw SyntaxError{"Invalid Symbol"};
}

Token Tokenizer::GetToken() {
    return curr_token_;
}

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

bool BooleanToken::operator==(const BooleanToken &other) const {
    return state == other.state;
}