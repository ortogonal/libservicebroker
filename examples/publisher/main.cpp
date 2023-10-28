#include <iostream>
#include <thread>

#include "batteryservice.h"
#include "redisservicepublisher.h"

int main()
{
    using namespace std;

    RedisServicePublisher redis;
    BatteryService battery(0, redis);

    for (int i = 0; i < 100; i++) {
        battery.setCharge(i);
        std::this_thread::sleep_for(500ms);
    }
}
