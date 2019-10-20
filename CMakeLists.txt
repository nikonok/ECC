cmake_minimum_required(VERSION 3.12.1)

project(ChiperedNote)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -DCRYPTOPP_DISABLE_ASM")


add_executable(find_params find_params.cpp utils.hpp)

add_executable(key_exchange key_exchange.cpp utils.hpp)

## CryptoPP
set(CryptoPP_DIR "cryptopp/include")
file(GLOB CryptoPP_SOURCES "cryptopp/include/cryptopp/*.cpp")
file(GLOB CryptoPP_HEADERS "cryptopp/include/cryptopp/*.h")
add_library(cryptopp STATIC ${CryptoPP_SOURCES} ${CryptoPP_HEADERS})

add_executable(signature signature.cpp sigutils.hpp)
target_link_libraries(signature PUBLIC cryptopp)
target_include_directories(signature PUBLIC ${CryptoPP_DIR})