#include <servicebroker/iservicedatapublisher.h>

#include <vector>

#include <sw/redis++/redis++.h>

using namespace sw;
namespace sb = servicebroker;

class RedisServicePublisher;
class RedisServicePublisherNode : public sb::IServicePublisherNode
{
public:
    RedisServicePublisherNode(RedisServicePublisher *creator,
                              const std::string &serviceName,
                              const int &instance);

    void publishProperty(const std::string_view &property, const sb::Variant &value) override;

private:
    RedisServicePublisher *m_creator = nullptr;
    const std::string m_serviceName;
    const int m_serviceInstance;
};

class RedisServicePublisherProxyNode : public sb::IServicePublisherNode
{
public:
    RedisServicePublisherProxyNode(std::shared_ptr<RedisServicePublisherNode> node);

    void publishProperty(const std::string_view &property, const sb::Variant &value) override;

private:
    const std::shared_ptr<RedisServicePublisherNode> m_node;
};

class RedisServicePublisher : public sb::IServiceDataPublisher
{
public:
    RedisServicePublisher();

    std::unique_ptr<sb::IServicePublisherNode> createPublisherNode(const std::string &serviceName,
                                                                   const int &instance) override;

private:
    void publishProperty(const std::string &serviceName,
                         const std::string_view &property,
                         const sb::Variant &value);

    std::vector<std::shared_ptr<RedisServicePublisherNode>> m_publisherNodes;

    redis::Redis m_redis;

    friend class RedisServicePublisherNode;
};
