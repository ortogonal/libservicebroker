#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace servicebroker {

enum class Type { intType, stringType, floatType, boolType };

using Variant = std::variant<int, std::string, float, bool>;

class IServiceSubscriber;
class IServiceSubscriberNode;

class IServiceSubscriberNode
{
public:
    virtual ~IServiceSubscriberNode() = default;

    virtual void onPropertyChange(
        std::function<void(const std::string &property, const Variant &value)> &&onChange)
        = 0;

    [[nodiscard]] virtual std::optional<Variant>
    getPropertyValue(const std::string_view &propertyName, const Type &type) = 0;
};

class IServiceDataSubscriber
{
public:
    virtual ~IServiceDataSubscriber() = default;

    virtual std::unique_ptr<IServiceSubscriberNode> createSubscriberNode(
        const std::string &serviceName,
        const int &instance,
        const std::unordered_map<std::string_view, Type> &propertyMap)
        = 0;
};

} // namespace servicebroker
