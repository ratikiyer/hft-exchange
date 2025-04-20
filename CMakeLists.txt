cmake_minimum_required(VERSION 3.14)
project(OrderbookProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


add_library(orderbook_lib
    src/orderbook.cpp
)

# ----------  protobuf / gRPC ----------
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_SRC proto/exchange.proto)
protobuf_generate_cpp(PROTO_CPP  PROTO_HDR  ${PROTO_SRC})
grpc_generate_cpp(GRPC_CPP GRPC_HDR ${PROTO_SRC})

add_library(proto_objs ${PROTO_CPP} ${PROTO_HDR} ${GRPC_CPP} ${GRPC_HDR})
target_link_libraries(proto_objs PUBLIC protobuf::libprotobuf gRPC::grpc++)

# ----------  matching engine lib ----------
add_library(engine_lib
    src/exchange/engine.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_CPP}
    ${CMAKE_CURRENT_BINARY_DIR}/${GRPC_CPP})
target_include_directories(engine_lib PUBLIC includes ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(engine_lib PUBLIC orderbook_lib proto_objs concurrentqueue)

# ----------  executables ----------
add_executable(ome src/exchange/ome_main.cpp)
target_link_libraries(ome PRIVATE engine_lib)

add_executable(gateway src/exchange/gateway_main.cpp)
target_link_libraries(gateway PRIVATE proto_objs grpc++)

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
