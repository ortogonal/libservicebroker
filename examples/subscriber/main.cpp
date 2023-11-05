#include <iostream>

#include <batteryservice_subscriber.h>
#include "redisservicedatasubscriber.h"

using namespace sw::redis;
using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    RedisServiceDataSubscriber subscriber;
    BatteryService batteryService1(0, subscriber);
    BatteryService batteryService2(1, subscriber);

    batteryService1.onPropertyChange(
        [&batteryService1](const IBatteryService::PropertyIdentifier &propId) {
            std::cout << "Change in battery 1 service" << std::endl;
            switch (propId) {
            case IBatteryService::PropertyIdentifier::present:
                std::cout << "  present:  " << batteryService1.present() << std::endl;
                break;
            case IBatteryService::PropertyIdentifier::charging:
                std::cout << "  charging: " << batteryService1.charging() << std::endl;
                break;
            case IBatteryService::PropertyIdentifier::charge:
                std::cout << "  charge:   " << batteryService1.charge() << std::endl;
                break;
            }
        });

    batteryService2.onPropertyChange([](const IBatteryService::PropertyIdentifier &propId) {
        std::cout << "Change in battery 2 service" << std::endl;
    });

    std::cout << "Battery 1" << std::endl;
    std::cout << "  present:  " << batteryService1.present() << std::endl;
    std::cout << "  charging: " << batteryService1.charging() << std::endl;
    std::cout << "  charge:   " << batteryService1.charge() << std::endl << std::endl;

    std::cout << "Battery 2" << std::endl;
    std::cout << "  present:  " << batteryService2.present() << std::endl;
    std::cout << "  charging: " << batteryService2.charging() << std::endl;
    std::cout << "  charge:   " << batteryService2.charge() << std::endl;

    while (true) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
