#include "redisservicedatasubscriber.h"

#include <algorithm>
#include <iostream>

#include <servicebroker/servicesubscriber.h>

RedisServiceSubscriberNodeProxy::RedisServiceSubscriberNodeProxy(
    std::shared_ptr<RedisServiceSubscriberNode> node)
    : m_subscriberNode(node)
{}

RedisServiceSubscriberNodeProxy::~RedisServiceSubscriberNodeProxy()
{
    if (m_subscriberNode != nullptr)
        m_subscriberNode->releaseNode();
}

void RedisServiceSubscriberNodeProxy::onPropertyChange(
    std::function<void(const std::string &, const sb::Variant &)> &&onChange)
{
    if (m_subscriberNode != nullptr)
        m_subscriberNode->onPropertyChange(std::move(onChange));
}

std::optional<sb::Variant> RedisServiceSubscriberNodeProxy::getPropertyValue(
    const std::string_view &propertyName, const sb::Type &type)
{
    if (m_subscriberNode != nullptr) {
        return m_subscriberNode->getPropertyValue(propertyName, type);
    }
    return std::nullopt;
}

RedisServiceSubscriberNode::RedisServiceSubscriberNode(
    RedisServiceDataSubscriber *creator,
    const std::string &serviceName,
    const int &instance,
    const std::unordered_map<std::string_view, sb::Type> &propertyMap)
    : m_creator(creator)
    , m_serviceName(serviceName)
    , m_serviceInstance(instance)
    , m_propertyMap(propertyMap)
{}

RedisServiceSubscriberNode::~RedisServiceSubscriberNode()
{
    if (m_creator != nullptr) {
        m_creator->unsubscribe(serviceInstanceString());
        m_creator->releaseNode(this);
    }
}

void RedisServiceSubscriberNode::onPropertyChange(
    std::function<void(const std::string &, const sb::Variant &)> &&onChange)
{
    if (m_creator != nullptr) {
        m_subscriberFunctions.push_back(onChange);

        if (m_subscriberFunctions.size() == 1) {
            m_creator->subscribe(serviceInstanceString());
        }
    }
}

std::optional<sb::Variant> RedisServiceSubscriberNode::getPropertyValue(
    const std::string_view &propertyName, const sb::Type &type)
{
    if (m_creator != nullptr) {
        return m_creator->getPropertyValue(serviceInstanceString(), propertyName, type);
    }
    return std::nullopt;
}

void RedisServiceSubscriberNode::releaseNode()
{
    if (m_creator != nullptr) {
        m_creator->releaseNode(this);
    }
}

void RedisServiceSubscriberNode::signalPropertyChange(const std::string &property)
{
    auto search = [property](const std::pair<std::string_view, sb::Type> propertyDef) {
        return property == propertyDef.first;
    };

    auto nodeIter = std::find_if(m_propertyMap.begin(), m_propertyMap.end(), search);
    if (nodeIter != m_propertyMap.end()) {
        auto value = m_creator->getPropertyValue(serviceInstanceString(),
                                                 nodeIter->first,
                                                 nodeIter->second);

        if (value.has_value()) {
            for (const auto &subscriber : m_subscriberFunctions) {
                subscriber(property, value.value());
            }
        }
    }
}

std::string RedisServiceSubscriberNode::serviceInstanceString() const
{
    return m_serviceName + ":" + std::to_string(m_serviceInstance);
}

void RedisServiceSubscriberNode::setCreatorInvalid()
{
    m_creator = nullptr;
}

RedisServiceDataSubscriber::RedisServiceDataSubscriber()
    : m_redis(createConnectionOptions())
    , m_redisSubscriber(m_redis.subscriber())
{
    m_redisSubscriber.on_message(
        [&](std::string channel, std::string msg) { signalChange(channel, msg); });

    m_redisSubscriber.on_error(
        [&](std::exception_ptr e) { std::cerr << "On error: " << std::endl; });

    m_signalThread = std::thread(&RedisServiceDataSubscriber::signalThreadWorker, this);
}

RedisServiceDataSubscriber::~RedisServiceDataSubscriber()
{
    m_signalThreadContinueToRun = false;
    m_signalCond.notify_one();
    m_signalThread.join();
}

std::unique_ptr<sb::IServiceSubscriberNode> RedisServiceDataSubscriber::createSubscriberNode(
    const std::string &serviceName,
    const int &instance,
    const std::unordered_map<std::string_view, sb::Type> &propertyMap)
{
    auto node = std::make_shared<RedisServiceSubscriberNode>(this,
                                                             serviceName,
                                                             instance,
                                                             propertyMap);
    m_subscriberNodes.push_back(node);

    return std::make_unique<RedisServiceSubscriberNodeProxy>(node);
}

void RedisServiceDataSubscriber::releaseNode(sb::IServiceSubscriberNode *node)
{
    auto search = [node](const std::shared_ptr<RedisServiceSubscriberNode> n) {
        return n.get() == node;
    };

    auto iter = std::find_if(m_subscriberNodes.begin(), m_subscriberNodes.end(), search);
    if (iter != m_subscriberNodes.end()) {
        m_subscriberNodes.erase(iter);
    }
}

void RedisServiceDataSubscriber::subscribe(const std::string &key)
{
    m_redisSubscriber.subscribe(key);
}

void RedisServiceDataSubscriber::unsubscribe(const std::string &key)
{
    m_redisSubscriber.unsubscribe(key);
}

std::optional<sb::Variant> RedisServiceDataSubscriber::getPropertyValue(
    const std::string &key, const std::string_view &propertyName, const sb::Type &type)
{
    try {
        auto valueFuture = m_redis.hget(key, propertyName);
        auto value = valueFuture.get();
        if (value.has_value()) {
            return sb::stringToVariant(type, value.value());
        } else {
            std::cout << "Missing value of property " << propertyName << std::endl;
        }

    } catch (const redis::Error &e) {
    };

    return std::nullopt;
}

redis::ConnectionOptions RedisServiceDataSubscriber::createConnectionOptions() const
{
    redis::ConnectionOptions opts;
    opts.host = "127.0.0.1";
    opts.port = 6379;
    opts.socket_timeout = std::chrono::milliseconds(1000);

    return opts;
}

void RedisServiceDataSubscriber::signalChange(const std::string channel, const std::string msg)
{
    std::unique_lock lock(m_signalMutex);
    m_notificationQueue.push(std::make_pair(channel, msg));
    m_signalCond.notify_one();
    lock.unlock();
}

void RedisServiceDataSubscriber::signalThreadWorker()
{
    while (m_signalThreadContinueToRun) {
        std::unique_lock lock(m_signalMutex);
        m_signalCond.wait(lock);

        // Do signaling
        while (!m_notificationQueue.empty()) {
            // TODO: Don't lock the code, lock the data!
            //       No need for the locks while processing
            const auto &channel = m_notificationQueue.front().first;
            const auto &msg = m_notificationQueue.front().second;

            auto search = [channel](const std::shared_ptr<RedisServiceSubscriberNode> node) {
                return (node != nullptr && node->serviceInstanceString() == channel);
            };

            auto node = std::find_if(m_subscriberNodes.begin(), m_subscriberNodes.end(), search);
            if (node != m_subscriberNodes.end()) {
                (*node)->signalPropertyChange(msg);
            }

            m_notificationQueue.pop();
        }

        lock.unlock();
    }
}
