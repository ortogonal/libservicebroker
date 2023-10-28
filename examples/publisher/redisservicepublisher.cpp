#include "redisservicepublisher.h"

#include <iostream>
#include <string>

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

RedisServicePublisherNode::RedisServicePublisherNode(RedisServicePublisher *creator,
                                                     const std::string &serviceName,
                                                     const int &instance)
    : m_creator(creator)
    , m_serviceName(serviceName)
    , m_serviceInstance(instance)
{}

void RedisServicePublisherNode::publishProperty(const std::string_view &property,
                                                const sb::Variant &value)
{
    if (m_creator != nullptr) {
        auto name = m_serviceName + ":" + std::to_string(m_serviceInstance);
        m_creator->publishProperty(name, property, value);
    }
}

RedisServicePublisherProxyNode::RedisServicePublisherProxyNode(
    std::shared_ptr<RedisServicePublisherNode> node)
    : m_node(node)
{}

void RedisServicePublisherProxyNode::publishProperty(const std::string_view &property,
                                                     const sb::Variant &value)
{
    if (m_node != nullptr) {
        m_node->publishProperty(property, value);
    }
}

RedisServicePublisher::RedisServicePublisher()
    : m_redis("tcp://127.0.0.1:6379")
{}

std::unique_ptr<sb::IServicePublisherNode> RedisServicePublisher::createPublisherNode(
    const std::string &serviceName, const int &instance)
{
    auto node = std::make_shared<RedisServicePublisherNode>(this, serviceName, instance);

    m_publisherNodes.push_back(node);

    return std::make_unique<RedisServicePublisherProxyNode>(node);
}

void RedisServicePublisher::publishProperty(const std::string &serviceName,
                                            const std::string_view &property,
                                            const sb::Variant &value)
{
    std::string valueString;
    std::visit(overloaded{
                   [&valueString](int arg) { valueString = std::to_string(arg); },
                   [&valueString](std::string arg) { valueString = arg; },
                   [&valueString](bool arg) { valueString = arg ? "true" : "false"; },
                   [&valueString](float arg) { valueString = std::to_string(arg); },
               },
               value);

    m_redis.hset(serviceName, property, valueString);
    m_redis.publish(serviceName, property);
}
