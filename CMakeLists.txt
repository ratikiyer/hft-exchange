cmake_minimum_required(VERSION 3.14)
project(OrderbookProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# REMOVED: No mention of ENABLE_LOGGING option or add_definitions.

add_library(orderbook_lib
    src/orderbook.cpp
)

target_include_directories(orderbook_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2  # or latest stable
)
FetchContent_MakeAvailable(catch2)

add_executable(test-orderbook
    tests/test_orderbook.cpp
)

target_link_libraries(test-orderbook
    PRIVATE
        orderbook_lib
        Catch2::Catch2WithMain
)

enable_testing()
add_test(NAME test-orderbook COMMAND test-orderbook)
