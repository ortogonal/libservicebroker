#include "batteryservice.h"

#include <algorithm>
#include <array>
#include <vector>

#include <iostream>

namespace sb = servicebroker;

const constexpr std::array<sb::PropertyData<IBatteryService::PropertyIdentifier>, 3> properties = {{
    {IBatteryService::PropertyIdentifier::present, "present", sb::Type::boolType},
    {IBatteryService::PropertyIdentifier::charging, "charging", sb::Type::boolType},
    {IBatteryService::PropertyIdentifier::charge, "charge", sb::Type::intType},
}};

BatteryService::BatteryService(sb::IServiceDataSubscriber &dataSubscriber, const int &instance)
    : ServiceSubscriber<IBatteryService::PropertyIdentifier, 3>("battery",
                                                                instance,
                                                                properties,
                                                                dataSubscriber)
{
    runAtConstruction();
}

void BatteryService::handlePropertyValue(const PropertyIdentifier &propertyId,
                                         const sb::Variant &value)
{
    switch (propertyId) {
    case IBatteryService::PropertyIdentifier::present:
        m_present = std::get<bool>(value);
        break;
    case IBatteryService::PropertyIdentifier::charging:
        m_charging = std::get<bool>(value);
        break;
    case IBatteryService::PropertyIdentifier::charge:
        m_charge = std::get<int>(value);
        break;
    }

    // TOOD: Move into subscriber class
    //       Maybe not possible due to interface IBatteryService
    for (auto &subscriber : m_subscriberFunctions) {
        subscriber(propertyId);
    }
}

bool BatteryService::present() const
{
    return m_present;
}

bool BatteryService::charging() const
{
    return m_charging;
}

int BatteryService::charge() const
{
    return m_charge;
}

bool BatteryService::onPropertyChange(std::function<void(const PropertyIdentifier &)> subscriberFunc)
{
    m_subscriberFunctions.push_back(subscriberFunc);
    return true;
}
