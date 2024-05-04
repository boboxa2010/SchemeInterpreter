#pragma once

#include <memory>
#include <vector>
#include "error.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;

    virtual std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        throw NotImplementedError(__PRETTY_FUNCTION__);
    }
    virtual std::string Serialize() {
        throw NotImplementedError(__PRETTY_FUNCTION__);
    }
    virtual std::shared_ptr<Object> MakeCopy() {
        throw NotImplementedError(__PRETTY_FUNCTION__);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Integer Functions

class IsNumberFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsEqualFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsGreaterFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsLessFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsGreaterEqualFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsLessEqualFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class AdditionFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class SubtractionFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class MultiplicationFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class DivisionFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class MaxFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class MinFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class AbsFunction : public Object {
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Boolean Functions

class IsBooleanFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class NotFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class AndFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class OrFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// List Functions

class IsPairFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsNullFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class IsListFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class ConsFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class CarFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class CdrFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class ListFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class ListRefFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

class ListTailFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Objects
class Number : public Object {
public:
    Number(int value);

    int GetValue() const;

    std::string Serialize() override;

    std::shared_ptr<Object> MakeCopy() override;

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name);

    const std::string& GetName() const;

    std::string Serialize() override;

    std::shared_ptr<Object> MakeCopy() override;

private:
    std::string name_;
};

class Boolean : public Object {
public:
    Boolean(bool state);

    bool GetState() const;

    std::string Serialize() override;

    std::shared_ptr<Object> MakeCopy() override;

private:
    bool state_;
};

class Cell : public Object {
public:
    Cell() = default;

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second);

    void SetFirst(std::shared_ptr<Object> first);

    void SetSecond(std::shared_ptr<Object> second);

    std::shared_ptr<Object> GetFirst() const;

    std::shared_ptr<Object> GetSecond() const;

    std::string Serialize() override;

private:
    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

class EmptyList : public Object {
public:
    std::string Serialize() override;
};

std::shared_ptr<Object> Evaluate(std::shared_ptr<Object> ast);

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
}