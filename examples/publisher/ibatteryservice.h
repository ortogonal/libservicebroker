#pragma once

class IBatteryService
{
public:
    enum class PropertyIdentifier { present, charging, charge };

    virtual ~IBatteryService() = default;

    [[nodiscard]] virtual bool present() const = 0;
    virtual void setPresent(const bool &present) = 0;

    [[nodiscard]] virtual bool charging() const = 0;
    virtual void setCharging(const bool &charging) = 0;

    [[nodiscard]] virtual int charge() const = 0;
    virtual void setCharge(const int &charge) = 0;
};
