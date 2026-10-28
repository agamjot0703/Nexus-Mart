cmake_minimum_required(VERSION 3.16)
project(NexusMart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nexus_mart
    src/main.cpp
    src/Product.cpp
    src/User.cpp
    src/ShoppingCart.cpp
    src/Order.cpp
    src/PaymentMethod.cpp
    src/FileManager.cpp
    src/Store.cpp
    src/ConsoleUI.cpp
)

target_include_directories(nexus_mart PRIVATE include)

if(MSVC)
    target_compile_options(nexus_mart PRIVATE /W4)
else()
    target_compile_options(nexus_mart PRIVATE -Wall -Wextra -Wpedantic)
endif()
