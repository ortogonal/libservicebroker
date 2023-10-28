#pragma once

#include <array>
#include <iostream>
#include <memory>

#include <servicebroker/iservicedatasubscriber.h>
#include <servicebroker/iservicesubscriber.h>

namespace servicebroker {

template<class T>
struct PropertyData
{
    T propertyId;
    std::string_view name;
    Type type;
};

template<class T, std::size_t NoProperties>
class ServiceSubscriber : public IServiceSubscriber
{
public:
    ServiceSubscriber(const std::string &serviceName,
                      const int &instance,
                      const std::array<PropertyData<T>, NoProperties> &properties,
                      IServiceDataSubscriber &dataSubscriber)
        : m_serviceName(serviceName)
        , m_instance(instance)
        , m_properties(properties)
    {
        std::unordered_map<std::string_view, Type> map;
        for (auto &property : m_properties) {
            map[property.name] = property.type;
        }

        m_subscriberNode = dataSubscriber.createSubscriberNode(m_serviceName, m_instance, map);
    }

    virtual ~ServiceSubscriber() = default;

protected:
    void runAtConstruction()
    {
        for (const auto &property : m_properties) {
            auto value = m_subscriberNode->getPropertyValue(property.name, property.type);
            if (value.has_value())
                handlePropertyValue(property.propertyId, value.value());
        }
        m_subscriberNode->onPropertyChange([&](const std::string &property, const Variant &value) {
            auto search = [property](const PropertyData<T> &propertyData) {
                return property == propertyData.name;
            };

            auto iter = std::find_if(m_properties.begin(), m_properties.end(), search);
            if (iter != m_properties.end()) {
                handlePropertyValue(iter->propertyId, value);
            }
        });
    }

    [[nodiscard]] std::string serviceName() const final override { return m_serviceName; }

    [[nodiscard]] virtual int serviceInstance() const final override { return m_instance; }

    /**
     * @brief handlePropertyValue
     * @param propertyId
     * @param value
     *
     * TODO: Send a vector/set of changed values so we can signal once with multiple changes
     * instead of calling change multiple times with one argument/change.
     */
    virtual void handlePropertyValue(const T &propertyId, const Variant &value) = 0;

private:
    const std::string m_serviceName;
    const int m_instance = 0;
    const std::array<PropertyData<T>, NoProperties> &m_properties;

    std::unique_ptr<IServiceSubscriberNode> m_subscriberNode;
};

} // namespace servicebroker
