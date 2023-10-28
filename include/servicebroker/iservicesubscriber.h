#pragma once

#include <string>

namespace servicebroker {

class IServiceSubscriber
{
public:
    virtual ~IServiceSubscriber() = default;

    [[nodiscard]] virtual int serviceInstance() const = 0;

    [[nodiscard]] virtual std::string serviceName() const = 0;
};

} // namespace servicebroker
