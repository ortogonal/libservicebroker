#pragma once

#include "iservicedatapublisher.h"
#include <string_view>

namespace servicebroker {

class ServicePublisher
{
public:
    ServicePublisher(const std::string &serviceName,
                     const int &instance,
                     IServiceDataPublisher &dataPublisher)
    {
        m_publisherNode = dataPublisher.createPublisherNode(serviceName, instance);
    }

    void publishProperty(const std::string_view &property, const Variant &value)
    {
        if (m_publisherNode != nullptr) {
            m_publisherNode->publishProperty(property, value);
        }
    }

private:
    std::unique_ptr<IServicePublisherNode> m_publisherNode;
};

} // namespace servicebroker
