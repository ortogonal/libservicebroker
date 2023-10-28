#pragma once

#include <optional>
#include <string>
#include <variant>

namespace servicebroker {

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

enum class Type { intType, stringType, floatType, boolType };

using Variant = std::variant<int, std::string, float, bool>;

static std::string variantToString(const Variant &value)
{
    std::string valueString;
    std::visit(overloaded{
                   [&valueString](int arg) { valueString = std::to_string(arg); },
                   [&valueString](std::string arg) { valueString = arg; },
                   [&valueString](bool arg) { valueString = arg ? "true" : "false"; },
                   [&valueString](float arg) { valueString = std::to_string(arg); },
               },
               value);

    return valueString;
}

static inline std::optional<Variant> stringToBool(const std::string &value)
{
    if (value == "true")
        return std::make_optional(true);
    else if (value == "false")
        return std::make_optional(false);
    else
        return std::nullopt;
}

static inline std::optional<Variant> stringToInt(const std::string &value)
{
    std::string::size_type sz;
    auto iValue = std::stoi(value, &sz);

    if (sz != 0) {
        return std::make_optional(iValue);
    } else {
        return std::nullopt;
    }
}

static inline std::optional<Variant> stringToFloat(const std::string &value)
{
    std::string::size_type sz;
    auto fValue = std::stof(value, &sz);

    if (sz != 0) {
        return std::make_optional(fValue);
    } else {
        return std::nullopt;
    }
}

static std::optional<Variant> stringToVariant(Type type, const std::string &value)
{
    switch (type) {
    case Type::intType:
        return stringToInt(value);
    case Type::stringType:
        return value;
    case Type::floatType:
        return stringToFloat(value);
    case Type::boolType:
        return stringToBool(value);
    };

    return std::nullopt;
}

} // namespace servicebroker
