#pragma once

#include "ibatteryservice.h"
#include <servicebroker/servicepublisher.h>

namespace sb = servicebroker;

class BatteryService : public IBatteryService, public sb::ServicePublisher
{
public:
    BatteryService(const int &instance, sb::IServiceDataPublisher &dataPublisher);

    // IBatteryService interface
    bool present() const override;
    void setPresent(const bool &present) override;
    bool charging() const override;
    void setCharging(const bool &charging) override;
    int charge() const override;
    void setCharge(const int &charge) override;

private:
    bool m_present = false;
    bool m_charging = false;
    int m_charge = 0;
};
