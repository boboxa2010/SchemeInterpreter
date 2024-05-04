#include <sstream>
#include "scheme.h"
#include "tokenizer.h"
#include "parser.h"

std::string Interpreter::Run(const std::string& string) {
    std::stringstream ss{string};
    Tokenizer tokenizer{&ss};

    auto obj = Read(&tokenizer);

    return Evaluate(obj)->Serialize();
}