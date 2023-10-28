#pragma once

#include <servicebroker/servicesubscriber.h>

#include "ibatteryservice.h"

namespace sb = servicebroker;

class BatteryService : public IBatteryService,
                       public sb::ServiceSubscriber<IBatteryService::PropertyIdentifier, 3>
{
public:
    BatteryService(sb::IServiceDataSubscriber &dataSubscriber, const int &instance);

    // IBatteryService interface
    bool present() const override;
    bool charging() const override;
    int charge() const override;
    bool onPropertyChange(std::function<void(const PropertyIdentifier &)> subscriberFunc) override;

protected:
    void handlePropertyValue(const IBatteryService::PropertyIdentifier &propertyId,
                             const sb::Variant &value) override;

private:
    bool m_present = false;
    bool m_charging = false;
    int m_charge = 0;

    const int m_instance = 0;

    std::vector<std::function<void(const PropertyIdentifier &)>> m_subscriberFunctions;
};
