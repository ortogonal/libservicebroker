cmake_minimum_required(VERSION 3.5)

project(ServiceBroker)

set(LIBSERVICEBROKER_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(publisher)
add_subdirectory(subscriber)
