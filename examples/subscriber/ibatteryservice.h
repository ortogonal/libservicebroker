#pragma once

#include <functional>

class IBatteryService
{
public:
    enum class PropertyIdentifier { present, charging, charge };

    virtual ~IBatteryService() = default;

    [[nodiscard]] virtual bool present() const = 0;

    [[nodiscard]] virtual bool charging() const = 0;

    [[nodiscard]] virtual int charge() const = 0;

    virtual bool onPropertyChange(std::function<void(const PropertyIdentifier &)> subscriberFunc) = 0;
};
