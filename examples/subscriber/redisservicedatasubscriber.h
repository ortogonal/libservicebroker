#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <sw/redis++/async_redis++.h>

#include <servicebroker/iservicedatasubscriber.h>

using namespace sw;
namespace sb = servicebroker;

class RedisServiceDataSubscriber;

class RedisServiceSubscriberNode : public sb::IServiceSubscriberNode
{
public:
    RedisServiceSubscriberNode(RedisServiceDataSubscriber *creator,
                               const std::string &serviceName,
                               const int &instance,
                               const std::unordered_map<std::string_view, sb::Type> &propertyMap);
    virtual ~RedisServiceSubscriberNode();

    // IServiceSubscriberNode interface
    void onPropertyChange(
        std::function<void(const std::string &, const sb::Variant &)> &&onChange) override;
    std::optional<sb::Variant> getPropertyValue(const std::string_view &propertyName,
                                                const sb::Type &type) override;

    void releaseNode();

    void signalPropertyChange(const std::string &property);

    std::string serviceInstanceString() const;

private:
    void setCreatorInvalid();

    RedisServiceDataSubscriber *m_creator = nullptr;
    const std::string m_serviceName;
    const int m_serviceInstance = 0;

    std::vector<std::function<void(const std::string &, const sb::Variant &)>> m_subscriberFunctions;
    const std::unordered_map<std::string_view, sb::Type> m_propertyMap;
};

class RedisServiceSubscriberNodeProxy : public sb::IServiceSubscriberNode
{
public:
    RedisServiceSubscriberNodeProxy(std::shared_ptr<RedisServiceSubscriberNode> node);
    virtual ~RedisServiceSubscriberNodeProxy();

    // IServiceSubscriberNode interface
    void onPropertyChange(
        std::function<void(const std::string &, const sb::Variant &)> &&onChange) override;
    std::optional<sb::Variant> getPropertyValue(const std::string_view &propertyName,
                                                const sb::Type &type) override;

private:
    std::shared_ptr<RedisServiceSubscriberNode> m_subscriberNode;

    friend class RedisServiceSubscriberNode;
};

class RedisServiceDataSubscriber : public sb::IServiceDataSubscriber
{
public:
    RedisServiceDataSubscriber();
    virtual ~RedisServiceDataSubscriber();

    std::unique_ptr<sb::IServiceSubscriberNode> createSubscriberNode(
        const std::string &serviceName,
        const int &instance,
        const std::unordered_map<std::string_view, sb::Type> &propertyMap) override;

private:
    std::optional<sb::Variant> getPropertyValue(const std::string &key,
                                                const std::string_view &propertyName,
                                                const sb::Type &type);
    void releaseNode(sb::IServiceSubscriberNode *node);

    void subscribe(const std::string &key);
    void unsubscribe(const std::string &key);

    redis::ConnectionOptions createConnectionOptions() const;

    void signalChange(const std::string channel, const std::string msg);
    void signalThreadWorker();

    // TODO: Maybe use a std::map
    std::vector<std::shared_ptr<RedisServiceSubscriberNode>> m_subscriberNodes;
    std::queue<std::pair<std::string, std::string>> m_notificationQueue;

    std::thread m_signalThread;
    std::mutex m_signalMutex;
    std::condition_variable m_signalCond;
    bool m_signalThreadContinueToRun = true;

    redis::AsyncRedis m_redis;
    redis::AsyncSubscriber m_redisSubscriber;

    friend class RedisServiceSubscriberNode;
};
