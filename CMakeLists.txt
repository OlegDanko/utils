cmake_minimum_required(VERSION 3.14)

project(utils LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(utils STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/ticket_storage/TicketDispenser.cpp
)

target_include_directories(
    utils
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)
