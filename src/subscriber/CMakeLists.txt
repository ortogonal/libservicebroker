cmake_minimum_required(VERSION 3.5)

project(servicebroker-subscriber LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (SRC_LIST
    servicesubscriber.cpp
)

set (HEADER_LIST
    ${LIBSERVICEBROKER_HEADERS_DIR}/servicebroker/iservicesubscriber.h
    ${LIBSERVICEBROKER_HEADERS_DIR}/servicebroker/servicesubscriber.h
)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${HEADER_LIST})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PUBLIC ../../include)

include(GNUInstallDirs)
install(TARGETS redis-publisher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
