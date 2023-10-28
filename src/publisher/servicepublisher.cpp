#include <servicebroker/servicepublisher.h>

namespace servicebroker {

ServicePublisher::ServicePublisher(const std::string &serviceName,
                                   const int &instance,
                                   IServiceDataPublisher &dataPublisher)
{
    m_publisherNode = dataPublisher.createPublisherNode(serviceName, instance);
}

void ServicePublisher::publishProperty(const std::string_view &property, const Variant &value)
{
    if (m_publisherNode != nullptr) {
        m_publisherNode->publishProperty(property, value);
    }
}

} // namespace servicebroker
