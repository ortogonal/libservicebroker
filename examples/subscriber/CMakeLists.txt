cmake_minimum_required(VERSION 3.5)

project(redis-subscriber LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(HIREDIS_HEADER hiredis)
find_library(HIREDIS_LIB hiredis)

find_path(REDIS_PLUS_PLUS_HEADER sw)
find_library(REDIS_PLUS_PLUS_LIB redis++)

add_executable(${PROJECT_NAME}
    main.cpp
    batteryservice.h batteryservice.cpp
    redisservicedatasubscriber.h redisservicedatasubscriber.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ../common)
target_include_directories(${PROJECT_NAME} PUBLIC ${HIREDIS_HEADER} ${REDIS_PLUS_PLUS_HEADER})
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${HIREDIS_LIB}
    PRIVATE ${REDIS_PLUS_PLUS_LIB}
    PRIVATE servicebroker-subscriber
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
