#include "batteryservice.h"

BatteryService::BatteryService(const int &instance, sb::IServiceDataPublisher &dataPublisher)
    : ServicePublisher("battery", instance, dataPublisher)
{}

bool BatteryService::present() const
{
    return m_present;
}

void BatteryService::setPresent(const bool &present)
{
    if (m_present != present) {
        m_present = present;
        publishProperty("present", m_present);
    }
}

bool BatteryService::charging() const
{
    return m_charging;
}

void BatteryService::setCharging(const bool &charging)
{
    if (m_charging != charging) {
        m_charging = charging;
        publishProperty("charging", m_charging);
    }
}

int BatteryService::charge() const
{
    return m_charge;
}

void BatteryService::setCharge(const int &charge)
{
    if (m_charge != charge) {
        m_charge = charge;
        publishProperty("charge", m_charge);
    }
}
