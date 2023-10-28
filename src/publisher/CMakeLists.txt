cmake_minimum_required(VERSION 3.5)

project(servicebroker-publisher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (HEADER_LIST
    ${LIBSERVICEBROKER_HEADERS_DIR}/servicebroker/iservicepublisher.h
    ${LIBSERVICEBROKER_HEADERS_DIR}/servicebroker/servicepublisher.h
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ../../include)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
