#pragma once

#include <memory>

#include <servicebroker-types.h>

namespace servicebroker {

class IServicePublisherNode
{
public:
    virtual ~IServicePublisherNode() = default;

    virtual void publishProperty(const std::string_view &property, const Variant &value) = 0;
};

class IServiceDataPublisher
{
public:
    virtual ~IServiceDataPublisher() = default;

    virtual std::unique_ptr<IServicePublisherNode>
    createPublisherNode(const std::string &serviceName, const int &instance) = 0;
};

} // namespace servicebroker
