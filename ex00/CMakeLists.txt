cmake_minimum_required(VERSION 3.16)
project(btc_proj CXX)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

# ライブラリ（テスト対象）
add_library(btc_lib
    src/BitcoinExchange.cpp
)
target_include_directories(btc_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

# 実行ファイル（課題要件：名前は btc）:contentReference[oaicite:8]{index=8}
add_executable(btc src/main.cpp)
target_link_libraries(btc PRIVATE btc_lib)

# GoogleTest 取得
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# テスト
enable_testing()
add_executable(tests tests/test_btc.cpp)
target_link_libraries(tests PRIVATE btc_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
