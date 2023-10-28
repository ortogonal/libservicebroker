#pragma once

#include "iservicedatapublisher.h"
#include <string_view>

namespace servicebroker {

class ServicePublisher
{
public:
    ServicePublisher(const std::string &serviceName,
                     const int &instance,
                     IServiceDataPublisher &dataPublisher);

    void publishProperty(const std::string_view &property, const Variant &value);

private:
    std::unique_ptr<IServicePublisherNode> m_publisherNode;
};

} // namespace servicebroker
