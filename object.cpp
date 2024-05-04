#include <unordered_map>
#include <numeric>
#include <functional>
#include "object.h"

namespace {
    std::unordered_map<std::string, std::shared_ptr<Object>> string_to_function = {
            {"number?", std::make_shared<IsNumberFunction>()},
            {"=", std::make_shared<IsEqualFunction>()},
            {">", std::make_shared<IsGreaterFunction>()},
            {"<", std::make_shared<IsLessFunction>()},
            {">=", std::make_shared<IsGreaterEqualFunction>()},
            {"<=", std::make_shared<IsLessEqualFunction>()},
            {"+", std::make_shared<AdditionFunction>()},
            {"-", std::make_shared<SubtractionFunction>()},
            {"*", std::make_shared<MultiplicationFunction>()},
            {"/", std::make_shared<DivisionFunction>()},
            {"max", std::make_shared<MaxFunction>()},
            {"min", std::make_shared<MinFunction>()},
            {"abs", std::make_shared<AbsFunction>()},
            {"boolean?", std::make_shared<IsBooleanFunction>()},
            {"not", std::make_shared<NotFunction>()},
            {"and", std::make_shared<AndFunction>()},
            {"or", std::make_shared<OrFunction>()},
            {"pair?", std::make_shared<IsPairFunction>()},
            {"null?", std::make_shared<IsNullFunction>()},
            {"list?", std::make_shared<IsListFunction>()},
            {"cons", std::make_shared<ConsFunction>()},
            {"car", std::make_shared<CarFunction>()},
            {"cdr", std::make_shared<CdrFunction>()},
            {"list", std::make_shared<ListFunction>()},
            {"list-ref", std::make_shared<ListRefFunction>()},
            {"list-tail", std::make_shared<ListTailFunction>()}};

    std::shared_ptr<Object> ArithmeticOp(const std::vector<std::shared_ptr<Object>>& numbers,
                                         size_t start_pos, std::shared_ptr<Object> init,
                                         std::function<int64_t(int64_t lhs, int64_t rhs)> op) {
    if (!Is<Number>(init)) {
    throw RuntimeError{"Incorrect Type for :" + std::string(__PRETTY_FUNCTION__)};
}
return std::make_shared<Number>(std::accumulate(
        numbers.begin() + start_pos, numbers.end(), As<Number>(init)->GetValue(),
        [&op](int64_t lhs, std::shared_ptr<Object> rhs) {
            if (!Is<Number>(rhs)) {
                throw RuntimeError{"Incorrect Type for :" + std::string(__PRETTY_FUNCTION__)};
            }
            return op(lhs, As<Number>(rhs)->GetValue());
        }));
}

std::shared_ptr<Object> CompareOp(const std::vector<std::shared_ptr<Object>>& numbers,
                                  size_t start_pos,
std::function<bool(int64_t lhs, int64_t rhs)> op) {
if (numbers.empty()) {
return std::make_shared<Boolean>(true);
}
if (!Is<Number>(numbers[0])) {
throw RuntimeError{"Not IntType" + std::string(__PRETTY_FUNCTION__)};
}
return std::make_shared<Boolean>(
        std::all_of(numbers.begin() + start_pos, numbers.end(), [&](auto arg) {
    if (!Is<Number>(arg)) {
        throw RuntimeError{"Not IntType" + std::string(__PRETTY_FUNCTION__)};
    }
    return op(As<Number>(numbers[0])->GetValue(), As<Number>(arg)->GetValue());
}));
}
}  // namespace

Number::Number(int value) : value_(value) {
}

int Number::GetValue() const {
    return value_;
}

std::string Number::Serialize() {
    return std::to_string(value_);
}

std::shared_ptr<Object> Number::MakeCopy() {
    return std::make_shared<Number>(value_);
}

Symbol::Symbol(const std::string& name) : name_(name) {
}

const std::string& Symbol::GetName() const {
    return name_;
}

std::string Symbol::Serialize() {
    return name_;
}

std::shared_ptr<Object> Symbol::MakeCopy() {
    return std::make_shared<Symbol>(name_);
}

Cell::Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
}

Boolean::Boolean(bool state) : state_(state) {
}

bool Boolean::GetState() const {
    return state_;
}

std::string Boolean::Serialize() {
    return state_ ? "#t" : "#f";
}

std::shared_ptr<Object> Boolean::MakeCopy() {
    return std::make_shared<Boolean>(state_);
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

void Cell::SetFirst(std::shared_ptr<Object> first) {
    first_ = first;
}

void Cell::SetSecond(std::shared_ptr<Object> second) {
    second_ = second;
}

std::string Cell::Serialize() {
    if (!first_ && !second_) {
        return "()";
    }
    std::string list = "(" + first_->Serialize();
    while (second_ && !Is<EmptyList>(second_)) {
        list += ' ';
        if (!Is<Cell>(second_) && !Is<EmptyList>(second_)) {
            list += ". ";
            list += second_->Serialize();
            break;
        }
        if (!Is<Cell>(second_)) {
            break;
        }
        if (As<Cell>(second_)->GetFirst()) {
            list += As<Cell>(second_)->GetFirst()->Serialize();
        }
        second_ = As<Cell>(second_)->GetSecond();
    }
    list += ')';
    return list;
}

void FillVectorOfArgs(std::shared_ptr<Object> object, std::vector<std::shared_ptr<Object>>& args) {
    if (!object) {
        return;
    }
    if ((Is<Cell>(object) && !As<Cell>(object)->GetFirst() && !As<Cell>(object)->GetSecond())) {
        args.push_back(std::make_shared<EmptyList>());
        return;
    }

    if (Is<Number>(object) || Is<Boolean>(object) || Is<Symbol>(object)) {
        args.push_back(object);
        return;
    }
    auto node = object;
    while (node && !Is<EmptyList>(node)) {
        if (!Is<Cell>(node)) {
            args.push_back(node);
            break;
        }
        if (Is<Cell>(As<Cell>(node)->GetFirst())) {
            args.push_back(Evaluate(As<Cell>(node)->GetFirst()));
        } else {
            args.push_back(As<Cell>(node)->GetFirst());
        }
        node = As<Cell>(node)->GetSecond();
    }
}
std::shared_ptr<Object> Evaluate(std::shared_ptr<Object> object) {
    if (!object) {
        throw RuntimeError{"Evaluating Nothing"};
    }
    if (Is<Number>(object) || Is<Boolean>(object) || Is<Symbol>(object)) {
        return object;
    }
    auto left = Evaluate(As<Cell>(object)->GetFirst());

    if (Is<Symbol>(left) && As<Symbol>(left)->GetName() == "quote") {
        if (!As<Cell>(object)->GetSecond()) {
            return std::make_shared<EmptyList>();
        }
        if ((Is<Cell>(As<Cell>(object)->GetSecond()) &&
             !As<Cell>(As<Cell>(object)->GetSecond())->GetFirst() &&
             !As<Cell>(As<Cell>(object)->GetSecond())->GetSecond())) {
            return std::make_shared<Cell>(std::make_shared<EmptyList>(), nullptr);
        }
        return std::shared_ptr<Object>(As<Cell>(object)->GetSecond());
    }
    std::vector<std::shared_ptr<Object>> args;
    FillVectorOfArgs(As<Cell>(object)->GetSecond(), args);

    if (Is<Symbol>(left) &&
        string_to_function.find(As<Symbol>(left)->GetName()) != string_to_function.end()) {
        return string_to_function[As<Symbol>(left)->GetName()]->Apply(args);
    }
    if (Is<Symbol>(left)) {
        return left;
    }
    throw RuntimeError{"Evaluating Wrong Type"};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Integer Functions

std::shared_ptr<Object> IsNumberFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return std::make_shared<Boolean>(Is<Number>(args[0]));
}

std::shared_ptr<Object> IsEqualFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    return CompareOp(args, 0, [](auto lhs, auto rhs) { return lhs == rhs; });
}

std::shared_ptr<Object> IsGreaterFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    return CompareOp(args, 1, [](auto lhs, auto rhs) { return lhs > rhs; });
}

std::shared_ptr<Object> IsLessFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    return CompareOp(args, 1, [](auto lhs, auto rhs) { return lhs < rhs; });
}

std::shared_ptr<Object> IsGreaterEqualFunction::Apply(
        const std::vector<std::shared_ptr<Object>>& args) {
    return CompareOp(args, 0, [](auto lhs, auto rhs) { return lhs >= rhs; });
}

std::shared_ptr<Object> IsLessEqualFunction::Apply(
        const std::vector<std::shared_ptr<Object>>& args) {
    return CompareOp(args, 0, [](auto lhs, auto rhs) { return lhs <= rhs; });
}

std::shared_ptr<Object> AdditionFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    return ArithmeticOp(args, 0, std::make_shared<Number>(0),
                        [](auto lhs, auto rhs) { return lhs + rhs; });
}

std::shared_ptr<Object> SubtractionFunction::Apply(
        const std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return ArithmeticOp(args, 1, args[0], [](auto lhs, auto rhs) { return lhs - rhs; });
}

std::shared_ptr<Object> MultiplicationFunction::Apply(
        const std::vector<std::shared_ptr<Object>>& args) {
    return ArithmeticOp(args, 0, std::make_shared<Number>(1),
                        [](auto lhs, auto rhs) { return lhs * rhs; });
}

std::shared_ptr<Object> DivisionFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return ArithmeticOp(args, 1, args[0], [](auto lhs, auto rhs) { return lhs / rhs; });
}

std::shared_ptr<Object> MaxFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    if (std::none_of(args.begin(), args.end(), [](auto object) { return Is<Number>(object); })) {
        throw RuntimeError{"Incorrect Type for :" + std::string(__PRETTY_FUNCTION__)};
    }
    return *std::max_element(args.begin(), args.end(), [](auto lhs, auto rhs) {
        return As<Number>(lhs)->GetValue() < As<Number>(rhs)->GetValue();
    });
}

std::shared_ptr<Object> MinFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    if (std::none_of(args.begin(), args.end(), [](auto object) { return Is<Number>(object); })) {
        throw RuntimeError{"Incorrect Type for :" + std::string(__PRETTY_FUNCTION__)};
    }
    return *std::min_element(args.begin(), args.end(), [](auto lhs, auto rhs) {
        return As<Number>(lhs)->GetValue() < As<Number>(rhs)->GetValue();
    });
}

std::shared_ptr<Object> AbsFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    if (!Is<Number>(args[0])) {
        throw RuntimeError{"Incorrect Type for :" + std::string(__PRETTY_FUNCTION__)};
    }
    return std::make_shared<Number>(std::abs(As<Number>(args[0])->GetValue()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Boolean Functions

std::shared_ptr<Object> IsBooleanFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return std::make_shared<Boolean>(Is<Boolean>(args[0]));
}
std::shared_ptr<Object> NotFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }

    return Is<Boolean>(args[0]) ? std::make_shared<Boolean>(!As<Boolean>(args[0])->GetState())
                                : std::make_shared<Boolean>(false);
}
std::shared_ptr<Object> AndFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    for (auto arg : args) {
        if (Is<Boolean>(arg) && !As<Boolean>(arg)->GetState()) {
            return arg;
        }
    }
    return args[args.size() - 1];
}
std::shared_ptr<Object> OrFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    for (auto arg : args) {
        if (Is<Boolean>(arg) && !As<Boolean>(arg)->GetState()) {
            continue;
        }
        if (!Is<EmptyList>(arg)) {
            return arg;
        }
    }
    return std::make_shared<Boolean>(false);
}

std::string EmptyList::Serialize() {
    return "()";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// List Functions

std::shared_ptr<Object> IsPairFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1 || !Is<Cell>(args[0])) {
        if (Is<EmptyList>(args[0])) {
            return std::make_shared<Boolean>(false);
        }
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    std::vector<std::shared_ptr<Object>> vector_arg;
    FillVectorOfArgs(args[0], vector_arg);
    return std::make_shared<Boolean>(vector_arg.size() == 2);
}
std::shared_ptr<Object> IsNullFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return std::make_shared<Boolean>(Is<EmptyList>(args[0]));
}
std::shared_ptr<Object> IsListFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1 || !Is<Cell>(args[0])) {
        if (Is<EmptyList>(args[0])) {
            return std::make_shared<Boolean>(true);
        }
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }

    auto object = args[0];

    while (object) {
        if (!Is<Cell>(object)) {
            return std::make_shared<Boolean>(false);
        }
        object = As<Cell>(object)->GetSecond();
    }
    return std::make_shared<Boolean>(true);
}
std::shared_ptr<Object> ConsFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return std::make_shared<Cell>(args[0], args[1]);
}
std::shared_ptr<Object> CarFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1 || !Is<Cell>(args[0]) ||
        (!As<Cell>(args[0])->GetFirst() && !As<Cell>(args[0])->GetSecond())) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    return As<Cell>(args[0])->GetFirst()->MakeCopy();
}
std::shared_ptr<Object> CdrFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 1 || !Is<Cell>(args[0]) ||
        (!As<Cell>(args[0])->GetFirst() && !As<Cell>(args[0])->GetSecond())) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    auto second = As<Cell>(args[0])->GetSecond();
    if (!second) {
        return std::make_shared<Cell>();
    }
    if (!Is<Cell>(second)) {
        return second->MakeCopy();
    }
    return second;
}
std::shared_ptr<Object> ListFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() == 1 && Is<EmptyList>(args[0])) {
        return args[0];
    }
    auto list = std::make_shared<Cell>();
    auto curr_node = list;
    for (auto object : args) {
        if (!curr_node->GetFirst()) {
            curr_node->SetFirst(object->MakeCopy());
        } else {
            curr_node->SetSecond(std::make_shared<Cell>(object->MakeCopy(), nullptr));
            curr_node = As<Cell>(curr_node->GetSecond());
        }
    }
    return list;
}
std::shared_ptr<Object> ListRefFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2 || !Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    std::vector<std::shared_ptr<Object>> vector_args;
    FillVectorOfArgs(args[0], vector_args);
    if (As<Number>(args[1])->GetValue() >= vector_args.size()) {
        throw RuntimeError{"Incorrect Value for index in :" + std::string(__PRETTY_FUNCTION__)};
    }
    return vector_args[As<Number>(args[1])->GetValue()];
}
std::shared_ptr<Object> ListTailFunction::Apply(const std::vector<std::shared_ptr<Object>>& args) {
    if (args.size() != 2 || !Is<Cell>(args[0]) || !Is<Number>(args[1])) {
        throw RuntimeError{"Invalid Number of Arguments for : " + std::string(__PRETTY_FUNCTION__)};
    }
    auto sub_list = As<Cell>(args[0]);
    size_t index = As<Number>(args[1])->GetValue();
    for (size_t i = 0; i < index; ++i) {
        sub_list = As<Cell>(sub_list->GetSecond());
        if (!sub_list && i != index - 1) {
            throw RuntimeError{"Incorrect Value for index in :" + std::string(__PRETTY_FUNCTION__)};
        }
    }
    if (!sub_list) {
        return std::make_shared<Cell>();
    }
    return sub_list;
}
